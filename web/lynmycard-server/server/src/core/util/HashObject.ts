import { createHash } from "crypto";

export default class HashObject {
  private hash: string;
  private exceptionArray: Array<string> = [];

  constructor() {
    this.addHashException("hash");
    this.addHashException("exceptionArray");
  }

  addHashException(key: string) {
    this.exceptionArray.push(key);
  }

  getHash() {
    if (!this.hash || this.hash.localeCompare("") === 0) {
      this.hash = "";
      const keys = Object.keys(this);
      const hash = createHash("md5");
      for (let i = 0; i < keys.length; i++) {
        const key = keys[i];
        if (this.exceptionArray.indexOf(key) >= 0) {
          continue;
        }
        hash.update(`${this[key]}`);
      }
      hash.update(this.constructor.name);
      this.hash = hash.digest("base64");
    }

    return this.hash;
  }
}
