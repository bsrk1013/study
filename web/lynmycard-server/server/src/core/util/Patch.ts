export const patch = (ns: object, path?: string) => {
  Object.keys(ns).forEach((key) => {
    const value = ns[key];
    const currentPath = path ? `${path}.${key}` : key;
    if (typeof value === "object") {
      patch(value, currentPath);
    }
    if (typeof value === "function") {
      Object.defineProperty(value, "name", {
        value: currentPath,
        configurable: true,
      });
    }
  });
};
