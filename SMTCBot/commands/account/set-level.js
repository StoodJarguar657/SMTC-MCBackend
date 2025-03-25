const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const UserManager = require("../../src/managers/UserManager");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("set-level")
        .setDescription("Sets the current permission level of a discord user")
        .addUserOption(option => option.setName("target").setDescription("The user to set it's permission level").setRequired(true))
        .addIntegerOption(option => option.setName("permission-level").setDescription("The new permissionLevel").setRequired(true))
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        /** @type {import("discord.js").User} */
        const target = interaction.options.getUser("target")
        const permissionLevel = interaction.options.getInteger("permission-level")
        
        if(target.bot)
            return await interaction.reply({ content: "Cannot set the permission level of a bot!", flags: MessageFlags.Ephemeral })

        if(!UserManager.IsValidPermissionLevel(permissionLevel))
            return await interaction.reply({ content: "Invalid Permission Level!", flags: MessageFlags.Ephemeral })

        const currentPermissionLevel = await UserManager.GetPermissionLevel(interaction.member.id)

        // Disable permission checking for developers
        if(currentPermissionLevel !== 2) {
            const targetPermissionLevel = await UserManager.GetPermissionLevel(target.id)
            if(targetPermissionLevel >= currentPermissionLevel)
                return await interaction.reply({ content: "You do not have the permission to modify this user's permission!", flags: MessageFlags.Ephemeral })

            if(permissionLevel == targetPermissionLevel)
                return await interaction.reply({ content: "Cannot change the permission level to the same level!", flags: MessageFlags.Ephemeral })
        }

        if(!await UserManager.UserExists(target.id))
            await UserManager.CreateUser(target.id)

        await UserManager.UpdatePermissionLevel(target.id, permissionLevel)
        await interaction.reply({ content: `Updated permission level to: **${UserManager.TranslateUserPermissionLevel(permissionLevel)}**`, flags: MessageFlags.Ephemeral })
    }
}