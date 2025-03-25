const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const UserManager = require("../../src/managers/UserManager");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("levels")
        .setDescription("Tells you all permission levels and their names")
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const translations = UserManager.GetTranslationNames()
        let message = "**Permission Levels:**\n\n```"

        const sortedPermLevels = Object.keys(translations).sort((a, b) => a - b);
        for (const permLevel of sortedPermLevels) {
            const permName = translations[permLevel]
            message += `\n${permLevel} -> ${permName}`
        }

        message += "\n```"

        await interaction.reply({ content: message, flags: MessageFlags.Ephemeral })
    }
}
