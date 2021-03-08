import ScheduleBase from "./ScheduleBase";
import { setInterval } from "timers";
import Log from "../Log";

export default class TimeScheduler {
  private static instance: TimeScheduler = new TimeScheduler();
  private scheduleMap: Object;

  static Instance() {
    return this.instance;
  }

  constructor() {
    this.init();
    console.log("TimeScheduler started...");
  }

  private init() {
    this.scheduleMap = new Object();
    setInterval(this.update, 10);
  }

  public reserve<T extends ScheduleBase>(e: T, reserveDate: Date) {
    const value = this.scheduleMap[e.getHash()];
    if (value) {
      return;
    }

    e.setRunDate(reserveDate);
    this.addSchedule<T>(e);
  }

  private addSchedule<T extends ScheduleBase>(e: T) {
    this.scheduleMap[e.getHash()] = e;
  }

  private deleteSchedule(key: string) {
    delete this.scheduleMap[key];
  }

  private update() {
    const now = Date.now();
    const scheduleMap = TimeScheduler.Instance().scheduleMap;

    const keys = Object.keys(scheduleMap);
    for (let i = 0; i < keys.length; i++) {
      const key = keys[i];
      if (!scheduleMap.hasOwnProperty(key)) {
        continue;
      }

      const e = scheduleMap[key] as ScheduleBase;

      if (!e) {
        TimeScheduler.Instance().deleteSchedule(key);
        continue;
      }

      const reserveDate = e.getRunDate();
      if (reserveDate.getTime() <= now) {
        TimeScheduler.Instance().deleteSchedule(key);

        e.run();
      }
    }
  }
}
