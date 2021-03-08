import { RowDataPacket } from "mysql2/promise";
import { MySqlDao } from "../db/MySqlDao";
import { MySqlConn } from "../db/MySqlDB";

export interface BookAuthorSchema extends RowDataPacket {
  id: number;
  bookId: number;
  authorId: number;
}

export class BookAuthorRepository extends MySqlDao<BookAuthorSchema> {
  constructor(mysql: MySqlConn, data: BookAuthorSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["bookId", "authorId"];
  }

  getTableName() {
    return "book_author";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new BookAuthorRepository(this.getMysql(), data);
  }
}
