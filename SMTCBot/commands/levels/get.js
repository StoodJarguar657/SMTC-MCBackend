import { MessageFlags, SlashCommandSubcommandBuilder } from "discord.js";
import database from "../../src/database.js";

export default {
    data: new SlashCommandSubcommandBuilder()
        .setName("get")
        .setDescription("Gets a level from a user")
        .addUserOption(option => option.setName("user").setDescription("The user to select").setRequired(true)),

    permissionLevel: 2,

    async init() {},

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const user = interaction.options.getUser("user");
        if(user.bot) {
            return await interaction.reply({ content: "You cannot select a bot as the user!", flags: MessageFlags.Ephemeral })
        }
        
        database.get("SELECT permLevel FROM users WHERE userId = ?", [user.id], async (err, row) => {
            if(err) {
                console.error(err);
                return await interaction.reply({ content: "Internal Server", flags: MessageFlags.Ephemeral });
            }

            if (!row) {
                return await interaction.reply({ content: `<@${user.id}>'s permission level is \`0\``, flags: MessageFlags.Ephemeral  });
            }
            
            await interaction.reply({ content: `<@${user.id}>'s permission level is \`${row.permLevel}\`.`,  flags: MessageFlags.Ephemeral });
        })
    }
}
