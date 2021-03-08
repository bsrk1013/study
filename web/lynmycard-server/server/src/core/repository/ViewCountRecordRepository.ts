import CacheRepository from "./CacheRepository";

export interface ViewCountDocument {
  id: string;
  viewCount: number;
}

export class ViewCountRecordReposioty extends CacheRepository {
  constructor(key: string) {
    super("LogDb", key);
  }
}
