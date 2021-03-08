import TimeUpdaterBase from "./TimeUpdaterBase";
import HashObject from "../HashObject";
import { Secconds } from "../../CoreConst";
import { ProductStore } from "../../store";
import Log from "../Log";
import Time from "../Time";
import PayLetter from "../PayLetter";
import { PurchaseLogType } from "../../repository";

export class AutopayInfo extends HashObject {
  private userId: number;
  private userEmail: string;
  private nextBillingDate: Date;
  private pgcode: string;
  private pid: number;
  private billingKey: string;

  /**
  Hash => userId 사용
   */
  constructor(
    userId: number,
    userEmail: string,
    nextBillingDate: Date,
    pgcode: string,
    pid: number,
    billingKey: string
  ) {
    super();
    this.addHashException("userEmail");
    this.addHashException("nextBillingDate");
    this.addHashException("pgcode");
    this.addHashException("pid");
    this.addHashException("billingKey");

    this.userId = userId;
    this.userEmail = userEmail;
    this.nextBillingDate = nextBillingDate;
    this.pgcode = pgcode;
    this.pid = pid;
    this.billingKey = billingKey;
  }

  public getUserId() {
    return this.userId;
  }

  public getUserEmail() {
    return this.userEmail;
  }

  public getNextBillingDate() {
    return this.nextBillingDate;
  }

  public setNextBillingDate(value: Date) {
    this.nextBillingDate = value;
  }

  public getPgcode() {
    return this.pgcode;
  }

  public getPid() {
    return this.pid;
  }

  public getBillingKey() {
    return this.billingKey;
  }
}

export default class AutopayManager extends TimeUpdaterBase {
  private static instance: AutopayManager;
  private autopayMap: Object;

  public static Instance(): AutopayManager {
    if (!this.instance) {
      this.instance = new AutopayManager();
    }
    return this.instance;
  }

  constructor() {
    super(1 * Secconds, "AutopayManager");
    this.init();
  }

  private async init() {
    this.autopayMap = new Object();

    const autopayList = await ProductStore.findAllAutopay();

    autopayList.forEach((info) => {
      this.insertAutopay(
        info.userId,
        info.userEmail,
        info.nextBillingDate,
        info.pgcode,
        info.pid,
        info.billingKey
      );
    });

    this.start();
  }

  protected async process() {
    const autopayMap = AutopayManager.Instance().autopayMap;
    if (!autopayMap) {
      return;
    }

    const values: AutopayInfo[] = Object.values(autopayMap);
    const now = Time.getNow();

    const targets = values.filter((c) => c.getNextBillingDate() <= now);
    setTimeout(() => {
      AutopayManager.Instance().processAutopay(targets);
    }, 0);
  }

  private async processAutopay(targets: AutopayInfo[]) {
    for (let i = 0; i < targets.length; i++) {
      let target = targets[i];

      if (!target) {
        Log.error("processAutopay, target is null");
        continue;
      }

      if (!target.getPid() || target.getPid() === 0) {
        this.removeAutopay(target.getHash());
        Log.error(
          `processAutopay, illegal pid, userId:${target.getUserId()}, pid:${target.getPid()}`
        );
        continue;
      }

      const subscription = await ProductStore.findOneBySubscription(
        target.getPid()
      );

      if (!subscription) {
        this.removeAutopay(target.getHash());
        Log.error(
          `processAutopay, subscription data is null, userId:${target.getUserId()}, pid:${target.getPid()}`
        );
        continue;
      }

      if (!subscription.isPeriodic) {
        this.removeAutopay(target.getHash());
        Log.error(
          `processAutopay, target is not periodic data, userId;${target.getUserId()}, pdi:${target.getPid()}`
        );
        continue;
      }

      const resultPrice =
        subscription.discountPrice !== -1
          ? subscription.discountPrice
          : subscription.price;

      const result = await PayLetter.Autopay(
        "웹툰에듀",
        target.getUserId(),
        target.getUserEmail(),
        target.getPgcode(),
        target.getPid(),
        target.getBillingKey(),
        subscription.name,
        resultPrice
      );

      if (!result) {
        Log.error(
          `autopay process failed, userId:${target.getUserId()}, pid:${target.getPid()}, billingKey:${target.getBillingKey()},`
        );
        this.removeAutopay(target.getHash());
        continue;
      }

      const resultData: PayLetter.AutopayResult = result.data;
      await ProductStore.savePurchaseLog(
        target.getUserId(),
        PurchaseLogType.Subscription,
        subscription.name,
        true,
        subscription.id,
        resultData.amount,
        0,
        resultData.cid,
        resultData.tid,
        target.getPgcode(),
        ""
      );

      const nextBillingDate = Time.convertToDateFromMsTs(
        target
          .getNextBillingDate()
          .setMonth(target.getNextBillingDate().getMonth() + 1)
      );

      await ProductStore.updateAutopay(
        target.getUserId(),
        nextBillingDate,
        resultData.tid,
        resultData.billkey
      );

      target.setNextBillingDate(nextBillingDate);
    }
  }

  public insertAutopay(
    userId: number,
    userEmail: string,
    nextBillingDate: Date,
    pgcode: string,
    pid: number,
    billingKey: string
  ) {
    const autopay = new AutopayInfo(
      userId,
      userEmail,
      nextBillingDate,
      pgcode,
      pid,
      billingKey
    );
    const key = autopay.getHash();

    if (this.autopayMap.hasOwnProperty(key)) {
      Log.error(
        `insert autopay error, already inserted, userId:${userId}, pid:${pid}, billingKey:${billingKey}`
      );
      return null;
    }

    this.autopayMap[key] = autopay;
    return autopay;
  }

  public getAutopay(key: string) {
    if (!this.autopayMap.hasOwnProperty(key)) {
      return null;
    }

    return this.autopayMap[key];
  }

  public removeAutopay(key: string) {
    delete this.autopayMap[key];
  }
}
