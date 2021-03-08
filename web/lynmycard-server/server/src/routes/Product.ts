import Express from "express";
import { ProductStore, UserStore } from "../core/store";
import { SubscriptionRewardType, PurchaseLogType } from "../core/repository";
import Time from "../core/util/Time";
import { Day, Hour } from "../core/CoreConst";
import { UserDto } from "../core/store/UserStore";
import Log from "../core/util/Log";
import PayLetter from "../core/util/PayLetter";
import { Configs, PayLetterConfig, ServerConfig } from "../Config";
import ErrorCode from "../core/ErrorCode";
import { SubscriptionDto } from "../core/store/ProductStore";
import AutopayManager, {
  AutopayInfo,
} from "../core/util/TimeUpdater/AutopayManager";
import { using } from "../core/util/Using";
import { ResponseScope } from "../Response";

const router = Express.Router();

const checkInfo = (userInfo: UserDto | null) => {
  if (!userInfo) {
    return false;
  }

  const now = Time.getTimeStamp();
  const expire = Time.convertToMsTsFromDate(userInfo.subscriptionExpireDate);
  return expire <= now;
};

router.get("/subscription", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const subscriptionList = await ProductStore.findBySubscription();

    scope.setData(subscriptionList);
    scope.setResult(true);
  });
});

router.post("/subscription/callback", async (req, resp, next) => {
  const receipt: PayLetter.Receipt = req.body;
  const customParameter: PayLetter.CustomParameter = JSON.parse(
    receipt.custom_parameter,
  );

  const userId = customParameter.userId;
  const pid = customParameter.pid;
  const autopayFlag = customParameter.autopayFlag;

  if (
    !PayLetter.VerifyReceipt(
      receipt.payhash,
      receipt.user_id,
      receipt.amount,
      receipt.tid,
    )
  ) {
    Log.error(receipt);
    await failAction(
      receipt,
      customParameter,
      null,
      ErrorCode.VERIFY_FAILED,
      resp,
      `receipt verify failed..., cid:${receipt.cid}, tid:${receipt.tid} userId:${userId}, pid:${pid}`,
    );
    return;
  }

  try {
    const userInfo = await UserStore.findOne(userId);
    const subscriptionInfo = await ProductStore.findOneBySubscription(pid);

    if (!userInfo) {
      await failAction(
        receipt,
        customParameter,
        subscriptionInfo,
        ErrorCode.INVALID_PARAMETER,
        resp,
        `illegal default user info, cid:${receipt.cid}, tid:${receipt.tid} userId:${userId}, pid:${pid}`,
      );
      return;
    }

    let startDate = Time.getNow();
    if (userInfo.isSubscription() && userInfo.subscriptionPid > 0) {
      startDate = new Date(userInfo.subscriptionStartDate);
    }

    let success = true;
    let novationDate = new Date(userInfo.subscriptionExpireDate);

    let novationTs = Time.convertToMsTsFromDate(novationDate);
    const now = Time.getTimeStamp();

    if (now > novationTs) {
      novationDate = Time.getNow();
    }

    if (subscriptionInfo.rewardType === SubscriptionRewardType.Month) {
      novationDate = Time.convertToDateFromMsTs(
        novationDate.setMonth(
          novationDate.getMonth() + subscriptionInfo.rewardValue,
        ),
      );
    } else if (subscriptionInfo.rewardType === SubscriptionRewardType.Day) {
      novationDate = Time.convertToDateFromMsTs(
        novationDate.setDate(
          novationDate.getDate() + subscriptionInfo.rewardValue,
        ),
      );
    } else if (subscriptionInfo.rewardType === SubscriptionRewardType.Hour) {
      novationDate = Time.convertToDateFromMsTs(
        novationDate.setHours(
          novationDate.getHours() + subscriptionInfo.rewardValue,
        ),
      );
    }

    success = await UserStore.update(
      userId,
      `subscriptionStartDate = '${Time.convertJsToMySqlFormat(
        startDate,
      )}', subscriptionExpireDate = '${Time.convertJsToMySqlFormat(
        novationDate,
      )}', subscriptionPid = ${pid}`,
    );

    if (!success) {
      await failAction(
        receipt,
        customParameter,
        subscriptionInfo,
        ErrorCode.INVALID_PARAMETER,
        resp,
        `illegal purchase, cid:${receipt.cid}, tid:${receipt.tid} userId:${userId},
         pid:${pid}, rewardType:${subscriptionInfo.rewardType}`,
      );
      return;
    }

    if (autopayFlag) {
      const autopayBillingKey = receipt.billkey;

      await ProductStore.saveAutopay(
        userId,
        Time.convertJsToMySqlFormat(novationDate),
        customParameter.pid,
        autopayBillingKey,
      );

      AutopayManager.Instance().insertAutopay(
        userId,
        userInfo.userEmail,
        novationDate,
        receipt.pgcode,
        pid,
        autopayBillingKey,
      );
    }

    await ProductStore.savePurchaseLog(
      userId,
      PurchaseLogType.Subscription,
      subscriptionInfo.name,
      subscriptionInfo.isPeriodic,
      subscriptionInfo.id,
      receipt.amount,
      receipt.tax_amount,
      receipt.cid,
      receipt.tid,
      receipt.pgcode,
      receipt.pay_info,
    );

    resp.send({
      code: 0,
      message: "billing success",
    });
  } catch (error) {
    await failAction(
      receipt,
      customParameter,
      null,
      ErrorCode.INTERNAL_DB_ERROR,
      resp,
      `cid:${receipt.cid}, tid:${receipt.tid} userId:${userId}, pid:${pid}\n${error}`,
    );
  }

  async function failAction(
    receipt: PayLetter.Receipt,
    customParameter: PayLetter.CustomParameter,
    subscriptionInfo: SubscriptionDto | null,
    errorCode: ErrorCode,
    resp: Express.Response,
    msg: string,
  ) {
    const result = await PayLetter.Cancel(
      receipt.pgcode,
      customParameter.userId,
      receipt.tid,
      receipt.amount,
    );

    if (result) {
      const cancelData = result.data;

      if (cancelData.tid && cancelData.tid.localeCompare("") !== 0) {
        await ProductStore.updatePurchaseLog(userId, receipt.tid);

        await ProductStore.savePurchaseCancelLog(
          userId,
          receipt.cid,
          receipt.tid,
          subscriptionInfo ? subscriptionInfo.name : "",
          customParameter.pid,
          receipt.amount,
          receipt.pgcode,
        );
      }
    }

    Log.error(msg);
    resp.send({
      code: errorCode,
      message: msg,
    });
  }
});

router.post("/subscription/return", async (req, resp, next) => {
  const receipt = req.body;
  const customParameter: PayLetter.CustomParameter = JSON.parse(
    receipt.custom_parameter,
  );

  const userId = customParameter.userId;
  const pid = customParameter.pid;
  const autopayFlag = customParameter.autopayFlag;

  if (
    !PayLetter.VerifyReceipt(
      receipt.payhash,
      receipt.user_id,
      receipt.amount,
      receipt.tid,
    )
  ) {
    resp.render("product/subscription/return", {
      message: "결제 인증에 실패했습니다.",
      to: `${ServerConfig.ClientOrigin}/product/subscription`,
    });
    return;
  }

  try {
    const userInfo = await UserStore.findOne(userId);
    const subscriptionInfo = await ProductStore.findOneBySubscription(pid);

    if (!userInfo) {
      resp.render("product/subscription/return", {
        message: "유저 정보에 문제가 있습니다.",
        to: `${ServerConfig.ClientOrigin}/product/subscription`,
      });
      return;
    }

    let success = false;
    switch (subscriptionInfo.rewardType) {
      case SubscriptionRewardType.Month:
      case SubscriptionRewardType.Day:
      case SubscriptionRewardType.Hour:
        success = true;
        break;
    }

    if (!success) {
      resp.render("product/subscription/return", {
        message: "잘못된 보상 타입입니다.",
        to: `${ServerConfig.ClientOrigin}/product/subscription`,
      });
      return;
    }

    resp.render("product/subscription/return", {
      message: "결제에 성공했습니다.",
      to: `${ServerConfig.ClientOrigin}/product/subscription`,
    });
  } catch (error) {
    Log.error(Log.makeMessageFromReq(req, error));
    resp.render("product/subscription/return", {
      message: "데이터베이스 오류 발생.",
      to: `${ServerConfig.ClientOrigin}/product/subscription`,
    });
  }
});

router.put("/subscription/free", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId: number = req.body.userId;
    const id: number = req.body.id;

    if (!userId || userId <= 0 || !id || id <= 0) {
      Log.error(
        `${scope.getName()}, illegal user id or pid, userId:${userId}, pid:${id}`,
      );
      scope.setResultCode(ErrorCode.INVALID_PARAMETER);
      return;
    }

    const userInfo = await UserStore.findOne(userId);
    const subscriptionInfo = await ProductStore.findOneBySubscription(id);

    if (!userInfo) {
      scope.setResultCode(ErrorCode.USER_NOT_FOUND);
      return;
    }

    if (!subscriptionInfo) {
      Log.error(
        `${scope.getName()}, subscription data is null, userId:${userId}, pid:${id}`,
      );
      scope.setResultCode(ErrorCode.NO_DATA);
      return;
    }

    let startDate = Time.getNow();
    // 이미 구독중인 경우 정기구독은 구매 불가
    // 정기 구독중인 경우 일반 구독은 구매 불가
    if (userInfo.isSubscription() && userInfo.subscriptionPid > 0) {
      const userProduct = await ProductStore.findOneBySubscription(
        userInfo.subscriptionPid,
      );
      if (
        (userProduct && userProduct.isPeriodic) ||
        subscriptionInfo.isPeriodic
      ) {
        Log.error(
          `${scope.getName()}, user already subscripted, userId:${userId}, pid:${
            userInfo.subscriptionPid
          }, expireDate:${userInfo.subscriptionExpireDate}`,
        );
        return;
      }
      startDate = new Date(userInfo.subscriptionStartDate);
    }

    let novationDate = new Date(userInfo.subscriptionExpireDate);
    let novationTs = Time.convertToMsTsFromDate(novationDate);
    const now = Time.getTimeStamp();

    if (now > novationTs) {
      novationDate = new Date(now);
    }
    if (subscriptionInfo.rewardType === SubscriptionRewardType.Month) {
      novationDate = Time.convertToDateFromMsTs(
        novationDate.setMonth(
          novationDate.getMonth() + subscriptionInfo.rewardValue,
        ),
      );
    } else if (subscriptionInfo.rewardType === SubscriptionRewardType.Day) {
      novationDate = Time.convertToDateFromMsTs(
        novationDate.setDate(
          novationDate.getDate() + subscriptionInfo.rewardValue,
        ),
      );
    } else if (subscriptionInfo.rewardType === SubscriptionRewardType.Hour) {
      novationDate = Time.convertToDateFromMsTs(
        novationDate.setHours(
          novationDate.getHours() + subscriptionInfo.rewardValue,
        ),
      );
    }

    await UserStore.update(
      userId,
      `subscriptionStartDate = '${Time.convertJsToMySqlFormat(
        startDate,
      )}', subscriptionExpireDate = '${Time.convertJsToMySqlFormat(
        novationDate,
      )}', subscriptionPid = ${id}`,
    );

    await ProductStore.savePurchaseLog(
      userId,
      PurchaseLogType.Subscription,
      subscriptionInfo.name,
      subscriptionInfo.isPeriodic,
      subscriptionInfo.id,
      subscriptionInfo.discountPrice === -1
        ? subscriptionInfo.price
        : subscriptionInfo.discountPrice,
      0,
      "free",
      "free",
      "free",
      "free",
    );

    scope.setResult(true);
  });
});

router.put("/subscription", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId: number = req.body.userId;
    const id: number = req.body.id;
    const pgcode: string = req.body.pgcode;

    if (!userId || userId <= 0 || !id || id <= 0 || !pgcode) {
      Log.error(
        `${scope.getName()}, illegal parameter, userId:${userId}, pid:${id}, pgcode:${pgcode}`,
      );
      scope.setResultCode(ErrorCode.INVALID_PARAMETER);
      return;
    }

    const userInfo = await UserStore.findOne(userId);
    const subscriptionInfo = await ProductStore.findOneBySubscription(id);

    if (!userInfo) {
      scope.setResultCode(ErrorCode.USER_NOT_FOUND);
      return;
    }

    if (!subscriptionInfo) {
      Log.error(
        `${scope.getName()}, subscription data is null, userId:${userId}, pid:${id}`,
      );
      scope.setResultCode(ErrorCode.NO_DATA);
      return;
    }

    // 이미 구독중인 경우 정기구독은 구매 불가
    // 정기 구독중인 경우 일반 구독은 구매 불가
    if (userInfo.isSubscription() && userInfo.subscriptionPid > 0) {
      const userProduct = await ProductStore.findOneBySubscription(
        userInfo.subscriptionPid,
      );
      if (
        (userProduct && userProduct.isPeriodic) ||
        subscriptionInfo.isPeriodic
      ) {
        Log.error(
          `${scope.getName()}, user already subscripted, userId:${userId}, pid:${
            userInfo.subscriptionPid
          }, expireDate:${userInfo.subscriptionExpireDate}`,
        );
        return;
      }
    }

    const resultPrice =
      subscriptionInfo.discountPrice !== -1
        ? subscriptionInfo.discountPrice
        : subscriptionInfo.price;

    let data = null;
    const result = await PayLetter.Request(
      req.baseUrl,
      pgcode,
      userId,
      userInfo.userEmail,
      "웹툰에듀",
      id,
      resultPrice,
      subscriptionInfo.name,
      userInfo.userEmail,
      `${Configs.env.localeCompare("development") === 0 ? "http" : "https"}://${
        PayLetterConfig.ApiUrl
      }/api/product/subscription/return`,
      `${Configs.env.localeCompare("development") === 0 ? "http" : "https"}://${
        PayLetterConfig.ApiUrl
      }/api/product/subscription/cancel`,
      `${Configs.env.localeCompare("development") === 0 ? "http" : "https"}://${
        PayLetterConfig.ApiUrl
      }/api/product/subscription/callback`,
      {
        userId: userId,
        userName: userInfo.userEmail,
        pid: id,
        autopayFlag: subscriptionInfo.isPeriodic ? true : false,
        host: req.baseUrl,
        expireDate: userInfo.subscriptionExpireDate,
      },
      true,
      subscriptionInfo.isPeriodic ? true : false,
    );

    if (result) {
      data = result.data;

      scope.setData(data);
      scope.setResult(true);
    }
  });
});

router.get("/purchaseLog", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId: number = req.body.userId;
    const index: number = Number.parseInt(req.query.index as string);

    if (!userId) {
      scope.setResultCode(ErrorCode.INVALID_PARAMETER);
      return;
    }

    const logList = await ProductStore.findByPurchaseLog(userId, index);
    const count = await ProductStore.countByPurchaseLog(userId);

    scope.setData({ logList, count });
    scope.setResult(true);
  });
});

router.get("/autopay", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId: number = req.body.userId;
    if (!userId) {
      scope.setResultCode(ErrorCode.INVALID_PARAMETER);
      return;
    }

    const autopayInfo = await ProductStore.findOneAutopay(userId);

    scope.setData(autopayInfo);
    scope.setResult(true);
  });
});

router.delete("/autopay", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId: number = req.body.userId;
    if (!userId) {
      scope.setResultCode(ErrorCode.INVALID_PARAMETER);
      return;
    }

    const autopayInfo = await ProductStore.findOneAutopay(userId);

    if (!autopayInfo) {
      Log.error(`${scope.getName()}, autopay info is null, userId:${userId}`);
      scope.setResultCode(ErrorCode.NO_DATA);
      return;
    }

    if (!autopayInfo.pid || autopayInfo.pid <= 0) {
      Log.error(`${scope.getName()}, illegal pid, pid:${autopayInfo.pid}`);
      scope.setResultCode(ErrorCode.INVALID_PARAMETER);
      return;
    }

    const subscriptionInfo = await ProductStore.findOneBySubscription(
      autopayInfo.pid,
    );

    const logInfo = await ProductStore.findOnePurchaseLogByTid(
      userId,
      autopayInfo.tid,
    );

    if (!logInfo) {
      Log.error(
        `${scope.getName()}, purchase log info is null, userId:${userId}, tid:${
          autopayInfo.tid
        }`,
      );
      scope.setResultCode(ErrorCode.NO_DATA);
      return;
    }

    const cancelResult = await PayLetter.Cancel(
      autopayInfo.pgcode,
      userId,
      autopayInfo.tid,
      logInfo.price,
    );

    if (cancelResult) {
      const cancelData = cancelResult.data;

      if (cancelData.tid && cancelData.tid.localeCompare("") !== 0) {
        await ProductStore.updatePurchaseLog(userId, autopayInfo.tid);

        await ProductStore.savePurchaseCancelLog(
          userId,
          cancelData.cid,
          cancelData.tid,
          subscriptionInfo ? subscriptionInfo.name : "",
          autopayInfo.pid,
          logInfo.price,
          autopayInfo.pgcode,
        );
      }

      await ProductStore.deleteAutopay(userId);

      const autopayKeyObject = new AutopayInfo(
        userId,
        "",
        Time.getNow(),
        "",
        0,
        "",
      );
      AutopayManager.Instance().removeAutopay(autopayKeyObject.getHash());
    }

    scope.setResult(true);
  });
});

export default router;
