import HashObject from "../HashObject";

export default class KeyItem3 extends HashObject {
  private key1: Object;
  private key2: Object;
  private key3: Object;

  constructor(key1: Object, key2: Object, key3: Object) {
    super();

    this.key1 = key1;
    this.key2 = key2;
    this.key3 = key3;
  }
}
