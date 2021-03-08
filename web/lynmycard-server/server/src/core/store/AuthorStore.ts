import { AuthorRepository, AuthorSchema } from "../repository";
import { using } from "../util/Using";
import { MySqlConn } from "../db/MySqlDB";
import Time from "../util/Time";
import ErrorCode from "../ErrorCode";
import { patch } from "../util/Patch";
export class AuthorDto {
  id: number;
  name: string;

  constructor(entity: AuthorSchema) {
    this.id = entity.id;
    this.name = entity.name;
  }
}

namespace AuthorStore {
  export async function findOne(id: number) {
    return await using(new MySqlConn(findOne.name), async (mysql) => {
      const authorEntity = await new AuthorRepository(mysql).findOne({ id });

      return new AuthorDto(authorEntity.getData());
    });
  }

  export async function findById(ids: number[]) {
    if (!ids || ids.length === 0) {
      return [];
    }
    return await using(new MySqlConn(findById.name), async (mysql) => {
      return await findByIdNoneUsing(mysql, ids);
    });
  }

  export async function findByIdNoneUsing(mysql: MySqlConn, ids: number[]) {
    if (!ids || ids.length === 0) {
      return [];
    }
    const authorEntities = await new AuthorRepository(mysql).findAllById(ids);
    return authorEntities.map((x) => new AuthorDto(x.getData()));
  }

  // TODO: mysql migration
  export async function findAllByKeyword(keyword: string) {
    return using(new MySqlConn(findAllByKeyword.name), async (mysql) => {
      const conn = mysql.getConn();

      // const list = await AuthorRepository.find({
      //   name: { $regex: `${keyword}` },
      // });

      return null;
    });
  }

  export async function findAll() {
    return using(new MySqlConn(findAll.name), async (mysql) => {
      const authorEntities = await new AuthorRepository(mysql).findAll({});
      return authorEntities.map((x) => new AuthorDto(x.getData()));
    });
  }

  export async function insert(name: string) {
    if (name.length === 0) {
      throw ErrorCode.INVALID_PARAMETER;
    }

    const regDate = Time.getNow();
    using(new MySqlConn(insert.name), async (mysql) => {
      const newAuthorEntity = new AuthorRepository(mysql, <AuthorSchema>{
        name,
        regDate,
      });

      await newAuthorEntity.insert();
    });
  }
}

patch({ AuthorStore });

export default AuthorStore;
