import { MySqlDao } from "../db/MySqlDao";
import { RowDataPacket } from "mysql2/promise";
import { MySqlConn } from "../db/MySqlDB";

export enum DeviceRegistrationType {
  Regist = 0,
  Temporary = 1,
}

export interface UserDeviceSchema extends RowDataPacket {
  id: number;
  userId: number;
  deviceId: string;
  deviceRegistrationType: number;
  deviceType: number;
  deviceName: string;
  deviceDefaultName: string;
  regDate: Date;
}

export class UserDeviceRepository extends MySqlDao<UserDeviceSchema> {
  constructor(mysql: MySqlConn, data: UserDeviceSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["userId", "deviceId"];
  }

  getTableName() {
    return "user_device";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new UserDeviceRepository(this.getMysql(), data);
  }

  async findAllByUserIdAndRegistrationType(
    userId: number,
    deviceRegistrationType: DeviceRegistrationType,
  ) {
    const mysql = this.getMysql();
    const rows = await mysql.query<UserDeviceSchema>(
      `select * from user_device where userId = ${userId} and deviceRegistrationType = ${deviceRegistrationType}`,
    );

    return rows[0].map((x) => new UserDeviceRepository(mysql, x));
  }
}
