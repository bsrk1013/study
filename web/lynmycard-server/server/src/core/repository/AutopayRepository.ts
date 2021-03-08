import { RowDataPacket } from "mysql2";
import { MySqlDao } from "../db/MySqlDao";
import { MySqlConn } from "../db/MySqlDB";

export interface AutopaySchema extends RowDataPacket {
  id: number;
  userId: number;
  userEmail: string;
  regDate: Date;
  nextBillingDate: Date;
  pgcode: string;
  pid: number;
  tid: string;
  billingKey: string;
}

export class AutopayRepository extends MySqlDao<AutopaySchema> {
  constructor(mysql: MySqlConn, data: AutopaySchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["userId"];
  }

  getTableName() {
    return "user_autopay";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new AutopayRepository(this.getMysql(), data);
  }
}
