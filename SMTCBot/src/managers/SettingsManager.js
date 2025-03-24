const filesystem = require("fs")

module.exports = {
    /** @returns {import("../../config.json")} */
    LoadSettings() {
        return JSON.parse(filesystem.readFileSync("config.json", "utf8"))
    },

    SaveSettings(settings) {
        filesystem.writeFileSync("config.json", JSON.stringify(settings), "utf8")
    }
}