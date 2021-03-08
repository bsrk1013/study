import Express from "express";
import { UserDto } from "../core/store/UserStore";
import { DeviceDto } from "../core/store/DeviceStore";

export async function CookieJsonParser(
  req: Express.Request,
  resp: Express.Response,
  next: Express.NextFunction,
) {
  let userId = 0;
  let userEmail = "";
  let deviceId = "";
  let publicIp = "";
  if (!req.cookies) {
  } else {
    if (req.cookies.cacheUserInfo) {
      const rawData = unescape(req.cookies.cacheUserInfo); // unescape로 디코딩 후 값 리턴
      const userInfo: UserDto = JSON.parse(rawData);

      userId = userInfo.id;
      userEmail = userInfo.userEmail;
    }
    if (req.cookies.cacheDeviceInfo) {
      const rawData = unescape(req.cookies.cacheDeviceInfo);
      const deviceInfo: DeviceDto = JSON.parse(rawData);

      deviceId = deviceInfo.deviceId;
    }
    if (req.cookies.publicIp) {
      publicIp = JSON.parse(req.cookies.publicIp);
    }
  }

  if (userId !== 0 && !req.body.userId) {
    req.body.userId = userId;
  }
  if (userEmail.length > 0 && !req.body.userEmail) {
    req.body.userEmail = userEmail;
  }
  if (deviceId.length > 0 && !req.body.deviceId) {
    req.body.deviceId = deviceId;
  }
  if (publicIp.length > 0 && !req.body.publicIp) {
    req.body.publicIp = publicIp;
  }

  next();
}
