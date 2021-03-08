import HashObject from "../HashObject";
import { Day, Minute } from "../../CoreConst";
import TimeUpdaterBase from "./TimeUpdaterBase";

export class SessionInfo extends HashObject {
  private userId: number;
  private regDate: number;

  constructor(userId: number, regDate: number) {
    super();
    this.userId = userId;
    this.regDate = regDate;
  }

  public getUserId() {
    return this.userId;
  }

  public getRegDate() {
    return this.regDate;
  }

  public setRegDate(newDate: number) {
    this.regDate = newDate;
  }
}

export default class SessionManager extends TimeUpdaterBase {
  private static instance: SessionManager = new SessionManager();
  private sessionMap: Object;
  private sessionExpireTime: number;

  static Instance(): SessionManager {
    return this.instance;
  }

  constructor() {
    super(1 * Minute, "SessionManager");

    this.sessionMap = new Object();
    this.sessionExpireTime = 1 * Day;

    this.start();
  }

  protected process() {
    const sessionMap = SessionManager.Instance().sessionMap;
    if (!sessionMap) {
      return;
    }

    const keys = Object.keys(sessionMap);
    const now = Date.now();
    for (let i = 0; i < keys.length; i++) {
      const key = keys[i];

      const session: SessionInfo = sessionMap[key];
      if (
        session.getRegDate() + SessionManager.Instance().sessionExpireTime >=
        now
      ) {
        SessionManager.Instance().removeSession(key);
      }
    }
  }

  public insertSession(userId: number): string | null {
    let session = new SessionInfo(userId, Date.now());
    const sessionToken = session.getHash();

    if (this.sessionMap.hasOwnProperty(sessionToken)) {
      return null;
    }

    this.sessionMap[sessionToken] = session;
    return sessionToken;
  }

  public getSession(sessionToken: string): SessionInfo | null {
    if (!this.sessionMap.hasOwnProperty(sessionToken)) {
      return null;
    }
    return this.sessionMap[sessionToken];
  }

  public removeSession(sessionToken: string) {
    delete this.sessionMap[sessionToken];
  }
}
