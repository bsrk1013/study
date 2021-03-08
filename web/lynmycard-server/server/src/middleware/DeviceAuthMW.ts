import Express from "express";
import { DeviceRegistrationType } from "../core/repository";
import DeviceStore from "../core/store/DeviceStore";
import { TemporaryDeviceExpireOffsetMs } from "../AppConst";

export default async function DeviceAuthMW(
  req: Express.Request,
  resp: Express.Response,
  next: Express.NextFunction,
) {
  const userEmail: string = req.body.userEmail;
  const deviceId: string = req.body.deviceId;

  const verifyAuthToken = async (userEmail: string, deviceId: string) => {
    if (!userEmail || !deviceId) {
      return false;
    }

    // const deviceInfo = await DeviceStore.findOne(userEmail, deviceId);
    // if (deviceInfo === null) {
    //   return false;
    // }

    // // 등록된 디바이스라면 인증 완료
    // if (deviceInfo.deviceRegistrationType == DeviceRegistrationType.Regist) {
    //   return true;
    // } else if (
    //   deviceInfo.deviceRegistrationType == DeviceRegistrationType.Temporary
    // ) {
    //   const remainMilliSeconds = deviceInfo.regDate.GetRemainMiliSeconds();
    //   if (remainMilliSeconds >= TemporaryDeviceExpireOffsetMs) {
    //     return false;
    //   }
    // }

    return true;
  };

  const result = verifyAuthToken(userEmail, deviceId);
  if (result) {
    next();
  } else {
    resp.send({ result: false });
  }
}
