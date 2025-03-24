const { SlashCommandBuilder } = require("discord.js");
const SubcommandManager = require("../src/managers/SubcommandManager");

module.exports = {
    permissionLevel: 0,
    data: new SlashCommandBuilder()
        .setName("players")
        .setDescription("Player management"),

    async Init() {
        await SubcommandManager.InitCommand(this, __filename)
    },

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        await SubcommandManager.RunCommand(this, interaction)
    }
}