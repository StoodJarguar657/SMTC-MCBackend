const path = require("path");

module.exports = (filePath) => {
    const callerDir = path.dirname(module.parent.filename);
    const resolvedPath = path.resolve(callerDir, filePath);
    
    delete require.cache[resolvedPath];

    return require(resolvedPath);
};