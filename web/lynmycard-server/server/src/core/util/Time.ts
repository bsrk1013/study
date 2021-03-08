export default class Time {
  private static defaultTime = new Date(0);

  static getDefaultTime() {
    return this.defaultTime;
  }

  static getDefaultTimeStamp() {
    return this.convertToMsTsFromDate(this.defaultTime);
  }

  static getTimeStamp() {
    return Number(new Date().getTime().toFixed(0));
  }

  static getNow() {
    return new Date();
  }

  static getToday() {
    const date = new Date();
    date.setHours(0);
    date.setMinutes(0);
    date.setSeconds(0);
    date.setMilliseconds(0);

    return date;
  }

  static getNextDay() {
    const date = this.getToday();
    date.setDate(date.getDate() + 1);
    return date;
  }

  static convertToMsTsFromDate(date: Date) {
    let ts = 0;
    try {
      ts = Number(date.getTime().toFixed(0));
    } catch (e) {
      const newDate = new Date(date);
      ts = Number(newDate.getTime().toFixed(0));
    }
    return ts;
  }

  static convertToDateFromMsTs(timestamp: number) {
    return new Date(timestamp);
  }

  static convertJsToMySqlFormat(date: Date) {
    function twoDigits(d) {
      if (0 <= d && d < 10) return "0" + d.toString();
      if (-10 < d && d < 0) return "-0" + (-1 * d).toString();
      return d.toString();
    }

    return (
      date.getFullYear() +
      "-" +
      twoDigits(1 + date.getMonth()) +
      "-" +
      twoDigits(date.getDate()) +
      " " +
      twoDigits(date.getHours()) +
      ":" +
      twoDigits(date.getMinutes()) +
      ":" +
      twoDigits(date.getSeconds())
    );

    // const dateArray = date.toISOString().split("T");
    // const prefix = dateArray[0];
    // const suffix = dateArray[1].split(".")[0];
    // return `${prefix} ${suffix}`;
  }

  static addDays(date: Date, days: number) {
    let newDate = new Date(date.valueOf());
    newDate.setDate(newDate.getDate() + days);
    return newDate;
  }

  static addHours = function (date: Date, hours: number) {
    let newDate = new Date(date.valueOf());
    newDate.setHours(newDate.getHours() + hours);
    return newDate;
  };

  static addMinutes = function (date: Date, minutes: number) {
    let newDate = new Date(date.valueOf());
    newDate.setMinutes(newDate.getMinutes() + minutes);
    return newDate;
  };

  static addSeconds = function (date: Date, seconds: number) {
    let newDate = new Date(date.valueOf());
    newDate.setSeconds(newDate.getSeconds() + seconds);
    return newDate;
  };

  static calculateTime(day: number, hour: number, min: number, sec: number) {
    const daySec = day * 24 * 60 * 60;
    const hourSec = hour * 60 * 60;
    const minSec = min * 60;
    return (daySec + hourSec + minSec + sec) * 1000;
  }
}
