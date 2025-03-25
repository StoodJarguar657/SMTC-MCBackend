const fs = require("fs/promises")

module.exports = {
    /**
     * @param {import("discord.js").Interaction} interaction 
     */
    async RunModal(interaction) {
        const modalsDirectory = (await fs.readdir("modals")).filter(file => file.endsWith(".js"))

        for(const modalFile of modalsDirectory) {
            if(modalFile.slice(0, -3) != interaction.customId)
                continue

            return await require(`../../modals/${modalFile}`)(interaction)
        }
    }
}