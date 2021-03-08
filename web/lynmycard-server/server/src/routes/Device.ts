import Express from "express";
import { MaxDeviceCount } from "../AppConst";
import { DeviceStore, UserStore } from "../core/store";
import { DeviceRegistrationType, UserDeviceSchema } from "../core/repository";
import Time from "../core/util/Time";
import { DeviceDto } from "../core/store/DeviceStore";
import { ResponseScope } from "../Response";
import { using } from "../core/util/Using";

const router = Express.Router();

router.get("/", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId: number = req.body.userId;
    const deviceList = await DeviceStore.find(userId);

    scope.setData(deviceList);
    scope.setResult(true);
  });
});

router.post("/", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId: number = req.body.userId;
    const userEmail: string = req.body.userEmail;
    const deviceId: string = req.body.deviceId;
    const deviceRegistrationType: number = req.body.deviceRegistrationType;
    const deviceType: number = req.body.deviceType;
    const deviceName: string = req.body.deviceName;
    const deviceDefaultName: string = req.body.deviceDefaultName;

    const canDeviceRegist = await checkInfo(
      userId,
      userEmail,
      deviceRegistrationType,
    );

    if (canDeviceRegist) {
      const now = Time.getNow();
      const deviceInfo = new DeviceDto(<UserDeviceSchema>{
        userId: userId,
        deviceId: deviceId,
        deviceRegistrationType: deviceRegistrationType,
        deviceType: deviceType,
        deviceName: deviceName,
        deviceDefaultName: deviceDefaultName,
        regDate: now,
      });

      if (deviceInfo.deviceRegistrationType === DeviceRegistrationType.Regist) {
        await DeviceStore.saveRegist(deviceInfo);
      } else if (
        deviceInfo.deviceRegistrationType === DeviceRegistrationType.Temporary
      ) {
        await DeviceStore.saveTemporary(deviceInfo);
      }
    }

    scope.setResult(true);
  });

  async function checkInfo(
    userId: number,
    userEmail: string,
    deviceRegistrationType: DeviceRegistrationType,
  ) {
    if (!userEmail) {
      const userInfo = await UserStore.findOne(userId);
      if (!userInfo) {
        return false;
      }
    } else {
      const existsUserInfo = await UserStore.exists(userEmail);
      if (!existsUserInfo) {
        return false;
      }
    }

    if (deviceRegistrationType === DeviceRegistrationType.Regist) {
      const deviceCount: number = await DeviceStore.countRegist(userId);
      if (deviceCount >= MaxDeviceCount) {
        return false;
      }
    }

    return true;
  }
});

router.delete("/", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId: number = Number.parseInt(req.query.userId as string);
    const deviceId: string = req.query.deviceId as string;

    await DeviceStore.remove(userId, deviceId);

    scope.setResult(true);
  });
});

router.put("/", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId: number = req.body.userId;
    const userEmail: string = req.body.userEmail;
    const deviceId = req.body.deviceId;
    const changeName = req.body.changeName;

    const existsUserInfo = await UserStore.exists(userEmail);
    if (existsUserInfo) {
      await DeviceStore.update(userId, deviceId, changeName);
    }

    scope.setResult(true);
  });
});

export default router;
