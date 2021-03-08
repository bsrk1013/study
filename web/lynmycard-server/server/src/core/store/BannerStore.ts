import {
  BannerRepository,
  BannerSchema,
  BannerType,
} from "../repository/BannerRepository";
import { using } from "../util/Using";
import { MySqlConn } from "../db/MySqlDB";
import { patch } from "../util/Patch";
import ErrorCode from "../ErrorCode";

namespace BannerStore {
  export class BannerDto {
    type: BannerType;
    imgPath: string;
    imgPath2: string;
    bookid: number;
    linkUrl: string;
    regDate: Date;
    priority: number;

    constructor(entity: BannerSchema) {
      this.type = entity.type;
      this.imgPath = entity.imgPath;
      this.imgPath2 = entity.imgPath2;
      this.bookid = entity.bookid;
      this.linkUrl = entity.linkUrl;
      this.regDate = entity.regDate;
      this.priority = entity.priority;
    }
  }

  export async function find(bundleKey: number) {
    return await using(new MySqlConn(find.name), async (mysql) => {
      const rows = await new BannerRepository(mysql).findAll({
        where: {
          bundleKey,
        },
      });

      if (!rows[0]) {
        return [];
      }

      return rows.map((c) => new BannerDto(c.getData()));
    });
  }
}

patch({ BannerStore });

export default BannerStore;
