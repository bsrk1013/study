import { MySqlDao } from "../db/MySqlDao";
import { RowDataPacket } from "mysql2/promise";
import { MySqlConn } from "../db/MySqlDB";

export interface ViewDetailLogSchema extends RowDataPacket {
  id: number;
  regDate: Date;
  userId: number;
  ip: string;
  bookId: number;
  episodeId: number;
  subscriptionPid: number;
  subscriptionName: string;
}

export interface ClientViewDetailLogSchema extends RowDataPacket {
  id: number;
  regDate: Date;
  userId: number;
  ip: string;
  // 웹툰 정보
  bookId: number;
  bookTitle: string;
  bookHash: string;
  bookThumbnailM: string;

  // 에피소드 정보
  episodeId: number;
  episodeTitle: string;
  episodeNo: number;

  // 구독권 정보
  subscriptionPid: number;
  subscriptionName: string;
}

export class ViewDetailLogRepository extends MySqlDao<ViewDetailLogSchema> {
  constructor(mysql: MySqlConn, data: ViewDetailLogSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["userId"];
  }

  getTableName() {
    return "view_detail_log";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new ViewDetailLogRepository(this.getMysql(), data);
  }
}
