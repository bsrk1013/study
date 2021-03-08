import { MySqlDao } from "../db/MySqlDao";
import { RowDataPacket } from "mysql2/promise";
import { MySqlConn } from "../db/MySqlDB";

export enum SubscriptionRewardType {
  Hour = 0,
  Day = 1,
  Month = 2,
}

export interface SubscriptionSchema extends RowDataPacket {
  id: number;
  name: string;
  isPeriodic: boolean;
  price: number;
  discountPrice: number;
  rewardType: number;
  rewardValue: number;
  isSubscription: number;
  description: string;
}

export class SubscriptionRepository extends MySqlDao<SubscriptionSchema> {
  constructor(mysql: MySqlConn, data: SubscriptionSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["id"];
  }

  getTableName() {
    return "tb_subscription";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new SubscriptionRepository(this.getMysql(), data);
  }
}
