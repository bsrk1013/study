import { RowDataPacket } from "mysql2/promise";
import { MySqlDao } from "../db/MySqlDao";
import { MySqlConn } from "../db/MySqlDB";

export interface BookSchema extends RowDataPacket {
  id: number;
  hash: string;
  title: string;
  description: string;
  authors: string;
  genres: string;
  isHide: number;
  day: string;
  regDate: Date;
  thumbnail_s: string;
  thumbnail_m: string;
  bannerImg: string;
  bannerImg_mobile: string;
  kakaoShareImg: string;
  orderNo: number;
  isShowMain: number;
}

export class BookRepository extends MySqlDao<BookSchema> {
  constructor(mysql: MySqlConn, data: BookSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["id"];
  }

  getTableName() {
    return "book";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new BookRepository(this.getMysql(), data);
  }

  async findAllByKeyword(keyword: string, start: number, count: number) {
    const mysql = this.getMysql();
    const rows = await mysql.query<BookSchema>(
      `select * from book where title like '%${keyword}%' or authors like '%${keyword}%' limit ${start}, ${count}`,
    );

    return rows[0].map((item) => new BookRepository(mysql, item));
  }

  async findAllOrderByLatestEpisode(day?: number) {
    const mysql = this.getMysql();
    const rows = await mysql.query<BookSchema & { date: string }>(
      `SELECT * from (
        SELECT a.*, MAX(b.availableDate) AS 'date'
        FROM book a
        LEFT JOIN episode b ON a.id = b.bookId
        WHERE b.availableDate <= NOW() AND b.isHide = 0
        GROUP BY a.id ORDER BY 'date' DESC) as x
        ${day ? `WHERE day like '%${day}%'` : ""}
        ORDER BY x.date DESC
        `,
    );

    return rows[0];
  }
}
