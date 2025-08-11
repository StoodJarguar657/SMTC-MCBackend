import { MessageFlags, SlashCommandSubcommandBuilder } from "discord.js";
import database from "../../src/database.js";

export default {
    data: new SlashCommandSubcommandBuilder()
        .setName("set")
        .setDescription("Sets a user's permission level")
        .addUserOption(option => option.setName("user").setDescription("The user to modify").setRequired(true))
        .addIntegerOption(option => option.setName("level").setDescription("The new permission level").setRequired(true)),

    permissionLevel: 2,

    async init() {},

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel // executor's level
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const targetUser = interaction.options.getUser("user");
        const newLevel = interaction.options.getInteger("level");

        if(targetUser.bot) {
            return await interaction.reply({ content: "You cannot select a bot as the user!", flags: MessageFlags.Ephemeral })
        }

        if(targetUser.id == interaction.user.id) {
            return await interaction.reply({ content: "You cannot set your own level!", flags: MessageFlags.Ephemeral })
        }


        database.get("SELECT permLevel FROM users WHERE userId = ?", [targetUser.id], async (err, row) => {
            if (err) {
                console.error(err);
                return await interaction.reply({ content: "Internal Server Error", flags: MessageFlags.Ephemeral });
            }

            const targetCurrentLevel = row ? row.permLevel : 0;

            if (targetCurrentLevel >= permissionLevel) {
                return await interaction.reply({ content: `You cannot modify the permission level of a user with equal or higher permission level than yourself.`, flags: MessageFlags.Ephemeral });
            }

            if (newLevel > permissionLevel) {
                return await interaction.reply({ content: `You cannot set a permission level higher than your own (${permissionLevel}).`, flags: MessageFlags.Ephemeral });
            }

            database.run(`INSERT INTO users (userId, permLevel) VALUES (?, ?) ON CONFLICT(userId) DO UPDATE SET permLevel = excluded.permLevel`, [targetUser.id, newLevel],async (err) => {
                if (err) {
                    console.error(err);
                    return await interaction.reply({ content: "Internal Server Error", flags: MessageFlags.Ephemeral });
                }

                await interaction.reply({ content: `Set <@${targetUser.id}>'s permission level to \`${newLevel}\`.`, flags: MessageFlags.Ephemeral });
            });
        });
    }
};
