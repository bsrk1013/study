import { DeviceRegistrationType } from "../repository";
import { using } from "../util/Using";
import { MySqlConn } from "../db/MySqlDB";
import { UserDeviceRepository, UserDeviceSchema } from "../repository";
import Log from "../util/Log";
import { patch } from "../util/Patch";

export class DeviceDto {
  userId: number;
  deviceId: string;
  deviceRegistrationType: number;
  deviceType: number;
  deviceName: string;
  deviceDefaultName: string;
  regDate: Date;

  constructor(scheme: UserDeviceSchema) {
    this.userId = scheme.userId;
    this.deviceId = scheme.deviceId;
    this.deviceRegistrationType = scheme.deviceRegistrationType;
    this.deviceType = scheme.deviceType;
    this.deviceName = scheme.deviceName;
    this.deviceDefaultName = scheme.deviceDefaultName;
    this.regDate = scheme.regDate;
  }

  toDao(mysql: MySqlConn) {
    return new UserDeviceRepository(mysql, <UserDeviceSchema>{
      userId: this.userId,
      deviceId: this.deviceId,
      deviceRegistrationType: this.deviceRegistrationType,
      deviceType: this.deviceType,
      deviceName: this.deviceName,
      deviceDefaultName: this.deviceDefaultName,
      regDate: this.regDate,
    });
  }
}

namespace DeviceStore {
  export async function find(userId: number): Promise<DeviceDto[]> {
    return await using(new MySqlConn(find.name), async (mysql) => {
      const devices = await new UserDeviceRepository(mysql).findAll({
        where: {
          userId,
        },
      });
      return devices.map((x) => new DeviceDto(x.getData()));
    });
  }

  export async function findRegist(userId: number): Promise<DeviceDto[]> {
    return await using(new MySqlConn(findRegist.name), async (mysql) => {
      const devices = await new UserDeviceRepository(
        mysql,
      ).findAllByUserIdAndRegistrationType(
        userId,
        DeviceRegistrationType.Regist,
      );

      return devices.map((c) => new DeviceDto(c.getData()));
    });
  }

  export async function countRegist(userId: number): Promise<number> {
    return await using(new MySqlConn(countRegist.name), async (mysql) => {
      const count = await new UserDeviceRepository(mysql).count({
        where: {
          userId,
        },
      });

      return count;
    });
  }

  export async function findOne(
    userId: number,
    deviceId: string,
  ): Promise<DeviceDto | null> {
    return await using(new MySqlConn(findOne.name), async (mysql) => {
      const device = await new UserDeviceRepository(mysql).findOne({
        userId,
        deviceId,
      });

      return new DeviceDto(device.getData());
    });
  }

  export async function findTemporary(
    userId: number,
  ): Promise<DeviceDto | null> {
    return await using(new MySqlConn(findTemporary.name), async (mysql) => {
      return await findTemporaryNoneUsing(mysql, userId);
    });
  }

  export async function findTemporaryNoneUsing(
    mysql: MySqlConn,
    userId: number,
  ): Promise<DeviceDto | null> {
    const devices = await new UserDeviceRepository(
      mysql,
    ).findAllByUserIdAndRegistrationType(
      userId,
      DeviceRegistrationType.Temporary,
    );

    if (devices.length === 0) {
      return null;
    }

    return new DeviceDto(devices[0].getData());
  }

  export async function saveRegist(deviceInfo: DeviceDto) {
    return await using(new MySqlConn(saveRegist.name), async (mysql) => {
      await deviceInfo.toDao(mysql).insert();
    });
  }

  export async function saveTemporary(deviceInfo: DeviceDto) {
    return await using(new MySqlConn(saveTemporary.name), async (mysql) => {
      const temporary = await findTemporaryNoneUsing(mysql, deviceInfo.userId);
      if (!temporary) {
        await deviceInfo.toDao(mysql).insert();
      } else {
        await deviceInfo.toDao(mysql).update();
      }
    });
  }

  export async function remove(userId: number, deviceId: string) {
    return await using(new MySqlConn(remove.name), async (mysql) => {
      await new UserDeviceRepository(mysql).deleteAll({
        where: {
          userId,
          deviceId,
        },
      });
      return true;
    });
  }

  export async function exists(
    userId: number,
    deviceId: string,
  ): Promise<boolean> {
    return await using(new MySqlConn(exists.name), async (mysql) => {
      const result = await new UserDeviceRepository(mysql).findAll({
        where: { userId, deviceId },
      });

      return result.length !== 0;
    });
  }

  export async function update(
    userId: number,
    deviceId: string,
    changeName: string,
  ) {
    return await using(new MySqlConn(update.name), async (mysql) => {
      const device = await new UserDeviceRepository(mysql).findOne({
        userId,
        deviceId,
      });

      const data = device.getData();
      if (data) {
        data.deviceName = changeName;
        await device.update();
      }
    });
  }
}

patch({ DeviceStore });

export default DeviceStore;
