const fs = require("fs/promises")

module.exports = {
    /** @returns {import("../../config.json")} */
    async LoadSettings() {
        return JSON.parse(await fs.readFile("config.json", "utf8"))
    },

    async SaveSettings(settings) {
        await fs.writeFile("config.json", JSON.stringify(settings, null, 4), "utf8")
    }
}