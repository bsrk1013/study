import TimeUpdaterBase from "./TimeUpdaterBase";
import { Secconds } from "../../CoreConst";
import HashObject from "../HashObject";
import { SignUpAuthExpireOffsetMs } from "../../../AppConst";

export class AuthInfo extends HashObject {
  private userEmail: string;
  private deviceId: string;
  private authToken: number;
  private authType: number;
  private regDate: number;

  constructor(
    userEmail: string,
    deviceId: string,
    authToken: number,
    authType: number,
    regDate: number
  ) {
    super();

    this.addHashException("regDate");

    this.userEmail = userEmail;
    this.deviceId = deviceId;
    this.authToken = authToken;
    this.authType = authType;
    this.regDate = regDate;
  }

  public getRegDate() {
    return this.regDate;
  }

  public getAuthToken() {
    return this.authToken;
  }
}

export default class AuthManager extends TimeUpdaterBase {
  private static instance: AuthManager = new AuthManager();
  private authMap: Object;
  private authExpireTime: number;

  static Instance(): AuthManager {
    return this.instance;
  }

  constructor() {
    super(1 * Secconds, "AuthManager");

    this.authMap = new Object();
    this.authExpireTime = SignUpAuthExpireOffsetMs;

    this.start();
  }

  protected process() {
    const authMap = AuthManager.Instance().authMap;
    if (!authMap) {
      return;
    }

    const keys = Object.keys(authMap);
    const now = Date.now();
    for (let i = 0; i < keys.length; i++) {
      const key = keys[i];

      const auth: AuthInfo = authMap[key];
      if (!auth) {
        AuthManager.Instance().removeAuth(key);
        continue;
      }

      if (auth.getRegDate() + AuthManager.Instance().authExpireTime <= now) {
        AuthManager.Instance().removeAuth(key);
      }
    }
  }

  public verifyAuth(
    userEmail: string,
    deviceId: string,
    authToken: number,
    authType: number
  ) {
    const tempAuthInfo = new AuthInfo(
      userEmail,
      deviceId,
      authToken,
      authType,
      Date.now()
    );

    tempAuthInfo.getHash();
    console.log(tempAuthInfo);

    if (!this.authMap.hasOwnProperty(tempAuthInfo.getHash())) {
      return false;
    }

    const targetAuthInfo: AuthInfo = this.authMap[tempAuthInfo.getHash()];

    return targetAuthInfo.getAuthToken() === tempAuthInfo.getAuthToken();
  }

  public insertAuth(
    userEmail: string,
    deviceId: string,
    authToken: number,
    authType: number
  ) {
    let authInfo = new AuthInfo(
      userEmail,
      deviceId,
      authToken,
      authType,
      Date.now()
    );

    this.authMap[authInfo.getHash()] = authInfo;
    return authInfo;
  }

  public getAuth(key: string) {
    if (!this.authMap.hasOwnProperty(key)) {
      return null;
    }

    return this.authMap[key];
  }

  public removeAuth(key: string) {
    delete this.authMap[key];
  }
}
