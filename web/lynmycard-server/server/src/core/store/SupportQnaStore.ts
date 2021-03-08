import "../util/CommonExtensions";
import { using } from "../util/Using";
import { MySqlConn } from "../db/MySqlDB";
import {
  SupportQnaRepository,
  SupportQnaSchema,
} from "../repository/SupportQnaRepository";
import ErrorCode from "../ErrorCode";
import { patch } from "../util/Patch";

export class SupportQnaDto {
  id: number;
  userId: number;
  regDate: Date;
  state: number;
  qnaType: number;
  title: string;
  questionText: string;
  file1: string;
  file2: string;
  file3: string;
  answerDate: Date | null;
  answerText: string | null;

  constructor(entity: SupportQnaSchema) {
    this.id = entity.id;
    this.userId = entity.userId;
    this.regDate = entity.regDate;
    this.state = entity.state;
    this.qnaType = entity.qnaType;
    this.title = entity.title;
    this.questionText = entity.questionText;
    this.file1 = entity.file1;
    this.file2 = entity.file2;
    this.file3 = entity.file3;
    this.answerDate = entity.answerDate;
    this.answerText = entity.answerText;
  }

  toDao(mysql: MySqlConn) {
    return new SupportQnaRepository(mysql, <SupportQnaSchema>{
      id: this.id,
      userId: this.userId,
      regDate: this.regDate,
      state: this.state,
      qnaType: this.qnaType,
      title: this.title,
      questionText: this.questionText,
      file1: this.file1,
      file2: this.file2,
      file3: this.file3,
      answerDate: this.answerDate,
      answerText: this.answerText,
    });
  }
}

namespace SupportQnaStore {
  //내 문의 목록 조회
  export async function findAll(id: number) {
    return await using(new MySqlConn(findAll.name), async (mysql) => {
      const rows = await new SupportQnaRepository(mysql).findAll({
        where: {
          userid: id,
        },
      });

      if (rows.length === 0) {
        return [];
      }

      return rows.map((x) => new SupportQnaDto(x.getData()));
    });
  }

  //문의 가져오기
  export async function findOne(id: number, userId: number) {
    return await using(new MySqlConn(findOne.name), async (mysql) => {
      const rows = await new SupportQnaRepository(mysql).findAll({
        where: {
          id: id,
          userId: userId,
        },
      });

      if (rows.length === 0) {
        throw ErrorCode.NO_DATA;
      }

      return new SupportQnaDto(rows[0].getData());
    });
  }

  //문의 등록
  export async function insert(qna: SupportQnaDto): Promise<boolean> {
    return await using(new MySqlConn(insert.name), async (mysql) => {
      await new SupportQnaRepository(mysql, <SupportQnaSchema>{
        id: qna.id,
        userId: qna.userId,
        regDate: qna.regDate,
        state: qna.state,
        qnaType: qna.qnaType,
        title: qna.title,
        questionText: qna.questionText,
        file1: qna.file1,
        file2: qna.file2,
        file3: qna.file3,
        answerDate: qna.answerDate,
        answerText: qna.answerText,
      }).insert();

      return true;
    });
  }
}

patch({ SupportQnaStore });

export default SupportQnaStore;
