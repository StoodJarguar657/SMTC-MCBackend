const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js")
const CommandManager = require("../../src/managers/CommandManager")

module.exports = {
    permissionLevel: 2,
    data: new SlashCommandSubcommandBuilder()
        .setName("reload")
        .setDescription("Reloads all commands"),

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        await interaction.deferReply({
            flags: MessageFlags.Ephemeral
        });

        const isProduction = process.env.NODE_ENV === "production";
        const guildId = process.env.GUILD_ID;

        let success = isProduction ? await CommandManager.DeleteGuildCommands(guildId) : await CommandManager.DeleteCommands();
        if (!success) {
            return await interaction.editReply({
                content: "Failed to delete commands!",
            });
        }

        success = isProduction ? await CommandManager.ReloadCommands() : await CommandManager.ReloadGuildCommands(guildId);
        if (!success) {
            return await interaction.editReply({
                content: "Failed to reload commands! Please run `npm run deploy` or `npm run deploy:public` on the SMTC bot!",
            });
        }

        const successMessage = isProduction ? "Successfully reloaded commands!" : "Successfully reloaded commands for this guild!";
        await interaction.editReply({
            content: successMessage,
        });
    }
}
