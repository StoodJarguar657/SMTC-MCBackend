const { SlashCommandBuilder } = require("discord.js");
const SubcommandManager = require("../src/managers/SubcommandManager");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandBuilder()
        .setName("whitelist")
        .setDescription("Whitelist management"),

    async Init() {
        await SubcommandManager.InitCommand(this, __filename)
    },

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        await SubcommandManager.RunCommand(this, interaction)
    }
}