import ErrorCode from "../ErrorCode";

export interface IDisposable {
  init();
  dispose(valid: boolean, code: number);
}

export async function using<T extends IDisposable, U>(
  resource: T,
  action: (resource: T) => Promise<U>,
) {
  var valid = false;
  var code = ErrorCode.OK;
  try {
    await resource.init();
    const value = await action(resource);
    valid = true;
    return value;
  } catch (error) {
    if (typeof error === "number") {
      code = error;
    }

    throw error;
  } finally {
    resource.dispose(valid, code);
  }
}
