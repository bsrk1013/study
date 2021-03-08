export default abstract class TimeUpdaterBase {
  private interval: number;
  private updaterName: string;

  constructor(interval: number, updaterName: string) {
    this.interval = interval;
    this.updaterName = updaterName;
  }
  protected start() {
    setInterval(this.process, this.interval);

    console.log(`${this.updaterName} started...`);
  }

  protected abstract process();
}
