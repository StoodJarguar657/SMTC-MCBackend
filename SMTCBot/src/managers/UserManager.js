const databaseManager = require("./DatabaseManager")
const translation = {
    [-1]: "No permissions",
    [0]: "Minecraft Player",
    [1]: "Administrator",
    [2]: "Developer"
}

module.exports = {
    async CreateUser(userId) {
        return new Promise((resolve, reject) => {
            databaseManager.run(`INSERT INTO users (userId, permissionLevel) VALUES (?, ?)`, [userId.toString(), -1], (err) => {
                    if (err) reject(err);
                    resolve();
                }
            );
        });
    },

    async UserExists(userId) {
        const row = await new Promise((resolve, reject) => {
            databaseManager.get(`SELECT 1 FROM users WHERE userId = ? LIMIT 1`, [userId.toString()], (err, row) => {
                if (err) reject(err)

                resolve(row)
            })
        })

        return row !== undefined
    },

    async UpdatePermissionLevel(userId, newPermissionLevel) {
        return new Promise((resolve, reject) => {
            databaseManager.run(`UPDATE users SET permissionLevel = ? WHERE userId = ?`, [newPermissionLevel, userId], (err) => {
                if (err) reject(err)

                resolve()
            })
        })
    },

    async GetPermissionLevel(userId) {
        const row = await new Promise((resolve, reject) => {
            databaseManager.get(`SELECT permissionLevel FROM users WHERE userId = ?`, [userId], (err, row) => {
                if (err) reject(err)

                resolve(row)
            })
        })

        return row ? row.permissionLevel : -1
    },

    TranslateUserPermissionLevel(permissionLevel) {
        return translation[permissionLevel] || "Unknown"
    },

    GetTranslationNames() {
        return translation
    },

    IsValidPermissionLevel(permissionLevel) {
        return translation[permissionLevel] !== undefined
    }
}
