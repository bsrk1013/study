import TimeUpdaterBase from "./TimeUpdaterBase";
import { Day, Secconds } from "../../CoreConst";
import { UserStore } from "../../store";
import Time from "../Time";
import { SendMail, DormancyNotiMailForm, SendMailByAwsSES } from "../SendMail";
import Log from "../Log";
import { DormancyAccountMonth } from "../../../AppConst";

export default class DormancyManager extends TimeUpdaterBase {
  private static instance: DormancyManager;

  public static Instance(): DormancyManager {
    if (!this.instance) {
      this.instance = new DormancyManager();
    }
    return this.instance;
  }

  constructor() {
    super(1 * Day, "DormancyManager");
    this.init();
  }

  private async init() {
    this.start();
  }

  protected async process() {
    await DormancyManager.Instance().checkUser();
  }

  private async checkUser() {
    let targetDate = new Date(Time.getTimeStamp());
    targetDate.setMonth(targetDate.getMonth() - DormancyAccountMonth);

    const userList = await UserStore.findAllDormancyUser(targetDate);

    let userIdList: number[] = [];

    if (userList.length > 0) {
      userList.forEach(async (c) => {
        userIdList.push(c.id);

        Log.info(`send mail, userEmail:${c.userEmail}`);
        // SendMailByAwsSES(c.userEmail, DormancyNotiMailForm());
        SendMail(c.userEmail, DormancyNotiMailForm());
      });

      Log.info(`update dormancy, userIdList:${userIdList.join(",")}`);
      await UserStore.updateDormancyUser(userIdList.join(","));
    }
  }
}
