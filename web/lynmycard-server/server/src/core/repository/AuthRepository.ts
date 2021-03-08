import { MySqlDao } from "../db/MySqlDao";
import { RowDataPacket } from "mysql2/promise";
import { MySqlConn } from "../db/MySqlDB";

export enum AuthType {
  SignUp = 0,
  RemoveDevice = 1,
  TemporaryDevice = 2,
  FindPassword = 3,
}

export enum AuthResultType {
  Wait = 0,
  ExpireTime = 1,
  IllegalAuth = 2,
  NoAuth = 3,
  NoUser = 4,
  Success = 5,
}

export interface AuthSchema extends RowDataPacket {
  id: number;
  userId: number;
  authToken: string;
  regDate: Date;
}

export class AuthRepository extends MySqlDao<AuthSchema> {
  constructor(mysql: MySqlConn, data: AuthSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["userId"];
  }

  getTableName() {
    return "auth";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new AuthRepository(this.getMysql(), data);
  }
}
