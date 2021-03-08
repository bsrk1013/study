import "../util/CommonExtensions";
import { using } from "../util/Using";
import { MySqlConn } from "../db/MySqlDB";
import {
  SupportQnaTypeRepository,
  SupportQnaTypeSchema,
} from "../repository/SupportQnaTypeRepository";
import ErrorCode from "../ErrorCode";
import { patch } from "../util/Patch";

export class SupportQnaTypeDto {
  id: number;
  qnaType: number;
  typeName: string;

  constructor(entity: SupportQnaTypeSchema) {
    this.id = entity.id;
    this.qnaType = entity.qnaType;
    this.typeName = entity.typeName;
  }

  toDao(mysql: MySqlConn) {
    return new SupportQnaTypeRepository(mysql, <SupportQnaTypeSchema>{
      id: this.id,
      qnaType: this.qnaType,
      typeName: this.typeName,
    });
  }
}

namespace SupportQnaTypeStore {
  export async function findAll() {
    return await using(new MySqlConn(findAll.name), async (mysql) => {
      const rows = await new SupportQnaTypeRepository(mysql).findAll({});

      if (rows.length === 0) {
        throw ErrorCode.NO_DATA;
      }

      return rows.map((x) => new SupportQnaTypeDto(x.getData()));
    });
  }
}

patch({ SupportQnaTypeStore });

export default SupportQnaTypeStore;
