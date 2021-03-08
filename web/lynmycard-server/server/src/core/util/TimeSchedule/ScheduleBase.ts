import HashObject from "../HashObject";

export default abstract class ScheduleBase extends HashObject {
  private runDate: Date;

  constructor() {
    super();
    this.addHashException("runDate");
  }

  abstract run();

  public setRunDate(date: Date) {
    this.runDate = date;
  }

  public getRunDate() {
    return this.runDate;
  }
}
