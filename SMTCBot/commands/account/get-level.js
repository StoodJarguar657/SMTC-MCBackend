const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const UserManager = require("../../src/managers/UserManager");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("get-level")
        .setDescription("Gets the current permission level of a discord user")
        .addUserOption(option => option.setName("target").setDescription("The user to get it's permission level").setRequired(true))
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        /** @type {import("discord.js").User} */
        const target = interaction.options.getUser("target")
        if(target.bot)
            return await interaction.reply({ content: "Cannot get the permission level of a bot!", flags: MessageFlags.Ephemeral })

        const permissionLevel = await UserManager.GetPermissionLevel(target.id)
        await interaction.reply({ content: `User ${target.displayName || target.username} has the permission level: **${UserManager.TranslateUserPermissionLevel(permissionLevel)}**`, flags: MessageFlags.Ephemeral })
    }
}