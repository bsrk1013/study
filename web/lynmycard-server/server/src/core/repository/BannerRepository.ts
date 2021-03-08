import { MySqlDao } from "../db/MySqlDao";
import { RowDataPacket } from "mysql2/promise";
import { MySqlConn } from "../db/MySqlDB";

export enum BannerType {
  book = 0,
  external_url = 1,
}

export interface BannerSchema extends RowDataPacket {
  type: BannerType;
  imgPath: string;
  imgPath2: string;
  bookid: number;
  linkUrl: string;
  regDate: Date;
  priority: number;
}

export class BannerRepository extends MySqlDao<BannerSchema> {
  constructor(mysql: MySqlConn, data: BannerSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["id"];
  }

  getTableName() {
    return "banner";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new BannerRepository(this.getMysql(), data);
  }
}
