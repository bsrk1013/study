import Express from "express";
import { UserStore, EpisodeStore } from "../core/store";
import Time from "../core/util/Time";
import { EpisodeDto } from "../core/store/EpisodeStore";
import { using } from "../core/util/Using";
import { ResponseScope } from "../Response";

const router = Express.Router();

export enum UserVerifyResultType {
  Success = 0,
  FaileLogin = 1,
  FaileSubscription = 2,
  FaileDevice = 3,
  FailEpisodeInfo = 4,
  FaileSignUpAuth = 5,
  NotOpendEpisode = 6,
}

async function verifyUser(userId: number, deviceId: string) {
  const userInfo = await UserStore.findOne(userId);
  if (!userInfo) {
    return UserVerifyResultType.FaileLogin;
  }

  if (!userInfo.signUpAuthAgree) {
    return UserVerifyResultType.FaileSignUpAuth;
  }

  const now = Time.getTimeStamp();
  const expire = Time.convertToMsTsFromDate(userInfo.subscriptionExpireDate);
  if (expire <= now) {
    return UserVerifyResultType.FaileSubscription;
  }

  // 오픈 한시적으로 디바이스 인증 제거
  // const deviceInfo = await DeviceStore.findOne(userId, deviceId);
  // if (!deviceInfo) {
  //   return UserVerifyResultType.FaileDevice;
  // }

  // if (deviceInfo.deviceRegistrationType === DeviceRegistrationType.Temporary) {
  //   const remainMilliSeconds = deviceInfo.regDate.GetRemainMiliSeconds();
  //   if (remainMilliSeconds >= TemporaryDeviceExpireOffsetMs) {
  //     return UserVerifyResultType.FaileDevice;
  //   }
  // }

  return UserVerifyResultType.Success;
}

router.get("/book", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId = req.body.userId;
    const deviceId = req.body.deviceId;
    const episodeId = Number.parseInt(req.query.episodeId as string);

    let result = UserVerifyResultType.Success;

    const episodeInfo: EpisodeDto = await EpisodeStore.findOne(episodeId);
    if (episodeInfo && !episodeInfo.isFree) {
      result = await verifyUser(userId, deviceId);

      if (new Date(episodeInfo.availableDate) > Time.getNow()) {
        result = UserVerifyResultType.NotOpendEpisode;
      }
    } else if (!episodeInfo) {
      result = UserVerifyResultType.FailEpisodeInfo;
    }

    scope.setData({ result });
    scope.setResult(true);
  });
});

export default router;
