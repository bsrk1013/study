import { MySqlDao } from "../db/MySqlDao";
import { RowDataPacket } from "mysql2/promise";
import { MySqlConn } from "../db/MySqlDB";

export enum PurchaseLogType {
  Subscription = 0,
  Couopon = 1,
}

export interface PurchaseLogSchema extends RowDataPacket {
  regDate: Date;
  userId: number;
  purchaseLogType: number;
  name: string;
  isPeriodic: boolean;
  pid: string;
  price: number;
}

export class PurchaseLogRepository extends MySqlDao<PurchaseLogSchema> {
  constructor(mysql: MySqlConn, data: PurchaseLogSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["id"];
  }

  getTableName() {
    return "purchase_log";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new PurchaseLogRepository(this.getMysql(), data);
  }
}
