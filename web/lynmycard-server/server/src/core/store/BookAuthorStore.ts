import { BookAuthorRepository, BookAuthorSchema } from "../repository";
import { AuthorStore } from ".";
import { using } from "../util/Using";
import { MySqlConn } from "../db/MySqlDB";
import { patch } from "../util/Patch";

namespace BookAuthorStore {
  export async function findAuthorsByBookId(bookId: number) {
    return await using(
      new MySqlConn(findAuthorsByBookId.name),
      async (mysql) => {
        return await findAuthorsByBookIdNoneUsing(mysql, bookId);
      },
    );
  }

  export async function findAuthorsByBookIdNoneUsing(
    mysql: MySqlConn,
    bookId: number,
  ) {
    const bookAuthorEntities = await new BookAuthorRepository(mysql).findAll({
      where: {
        bookId,
      },
    });

    const authors = await AuthorStore.findByIdNoneUsing(
      mysql,
      bookAuthorEntities.map((x) => x.getData().authorId),
    );

    return authors;
  }

  export async function insert(bookId: number, authorId: number) {
    return await using(new MySqlConn(insert.name), async (mysql) => {
      await insertNoneUsing(mysql, bookId, authorId);
    });
  }

  export async function insertNoneUsing(
    mysql: MySqlConn,
    bookId: number,
    authorId: number,
  ) {
    const entity = await new BookAuthorRepository(mysql, <BookAuthorSchema>{
      bookId,
      authorId,
    });
    await entity.insert();
  }
}

patch({ BookAuthorStore });

export default BookAuthorStore;
