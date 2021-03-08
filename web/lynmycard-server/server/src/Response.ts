import ErrorCode from "./core/ErrorCode";
import { IDisposable } from "./core/util/Using";
import Log from "./core/util/Log";

interface Response {
  success: boolean;
  data: object | null;
  resultCode: ErrorCode;
}

export class ResponseScope implements IDisposable {
  private slowQueryTime: number;
  private startTime: number;
  private resp: any;
  private result: Response = {
    data: null,
    success: false,
    resultCode: ErrorCode.OK,
  };

  constructor(resp: any, slowQueryTime: number = 1000) {
    this.resp = resp;
    this.slowQueryTime = slowQueryTime;
  }

  public setResult(result: boolean) {
    this.result.success = result;
  }

  public setData(data: object | null) {
    this.result.data = data;
  }

  public setResultCode(resultCode: ErrorCode) {
    this.result.resultCode = resultCode;
  }

  public getName(): string {
    return this.resp.req.originalUrl;
  }

  init() {
    this.startTime = Date.now();
  }

  dispose(valid: boolean, code: number) {
    const now = Date.now();
    const elapsed = now - this.startTime;

    if (this.slowQueryTime <= elapsed) {
      Log.warn(`slow scope... elapsed:${elapsed}\nname:${this.getName()}}`);
    }

    if (code !== ErrorCode.OK) {
      this.setResultCode(code);
    }

    // if (!this.result.success) {
    //   Log.error(`name:${this.getName()}\n${code as ErrorCode}`);
    // }

    // this.setResult(valid);

    this.resp.send(this.result);
  }
}
