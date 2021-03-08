import { RowDataPacket } from "mysql2/promise";
import { MySqlDao } from "../db/MySqlDao";
import { MySqlConn } from "../db/MySqlDB";

export enum LoginType {
  Default = 0,
  AccessToken = 1,
}

export enum UserType {
  None = 0,
  Admin = 1,
  User = 2,
}

export interface UserSchema extends RowDataPacket {
  id: number;
  userEmail: string;
  password: string;
  userType: number;
  name: string;
  lastLoginDate: Date;
  lastLogoutDate: Date;
  subscriptionStartDate: Date;
  subscriptionExpireDate: Date;
  subscriptionPid: number;
  privacyAgree: number;
  signUpAuthAgree: number;
  isDormancy: number;
}

export class UserRepository extends MySqlDao<UserSchema> {
  constructor(mysql: MySqlConn, data: UserSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["userEmail", "id"];
  }

  getTableName() {
    return "user";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new UserRepository(this.getMysql(), data);
  }
}
