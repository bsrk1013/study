import { FavoriteBookRepository } from "../repository";

namespace FavoriteStore {
  export async function findBookFavorite(userId: number) {
    const suffixKey = `:${userId}`;
    const favorites = await FavoriteBookRepository.smembersAsync<number>(
      suffixKey,
    );
    return favorites;
  }

  export async function updateBookFavorite(userId: number, bookId: number) {
    const suffixKey = `:${userId}`;
    const bookIdStr = bookId.toString();
    const exists = await FavoriteBookRepository.sismemberAsync(
      bookIdStr,
      suffixKey,
    );

    let remove = false;
    if (exists) {
      remove =
        (await FavoriteBookRepository.sremAsync(bookIdStr, suffixKey)) === 1;
    } else {
      await FavoriteBookRepository.setAsync(bookIdStr, suffixKey);
    }

    return remove;
  }

  export async function removeBookFavorite(userId: number) {
    const suffixKey = `:${userId}`;

    await FavoriteBookRepository.sremAsync(suffixKey);
  }

  export async function updateBookFavoriteEpisode(
    userId: number,
    bookId: number,
  ) {
    const suffixKey = `:${userId}`;
    const bookIdStr = bookId.toString();

    await FavoriteBookRepository.setAsync(bookIdStr, suffixKey);
  }
}

export default FavoriteStore;
