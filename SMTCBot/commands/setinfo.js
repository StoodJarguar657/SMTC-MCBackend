import { SlashCommandBuilder, ChatInputCommandInteraction, MessageFlags } from "discord.js";
import database from "../src/database.js";

export default {
    data: new SlashCommandBuilder()
        .setName("set-info")
        .setDescription("Sets information of the server")
        .addStringOption(option =>
            option
                .setName("variable")
                .setDescription("Variable to set")
                .setChoices(
                    { name: "Name", value: "Name" },
                    { name: "Description", value: "Description" },
                    { name: "Expose Public IP", value: "ExposePublicIp" },
                    { name: "Server Address", value: "ServerAddress" }
                )
                .setRequired(true)
        )
        .addStringOption(option =>
            option
                .setName("string-value")
                .setDescription("The value to set the variable to (for text values)")
                .setRequired(false)
        )
        .addBooleanOption(option =>
            option
                .setName("boolean-value")
                .setDescription("The boolean value to set (for Expose Public IP)")
                .setRequired(false)
        ),

    permissionLevel: 2,

    async init() {},

    /**
     * @param {ChatInputCommandInteraction} interaction
     * @param {import("serverInfo").ServerInfo} serverInfo
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const variableMap = {
            Name: "displayName",
            Description: "description",
            ExposePublicIp: "exposePublicIp",
            ServerAddress: "address"
        };

        const variableKey = interaction.options.getString("variable");
        const dbColumn = variableMap[variableKey];

        if (!dbColumn) {
            await interaction.reply({ content: `Invalid variable: \`${variableKey}\``, flags: MessageFlags.Ephemeral });
            return;
        }

        let newValue = null;

        if (variableKey === "ExposePublicIp") {
            const boolVal = interaction.options.getBoolean("boolean-value");
            if (typeof boolVal !== "boolean") {
                return await interaction.reply({ content: "Please provide a `boolean-value` (true or false) for `Expose Public IP`.", flags: MessageFlags.Ephemeral });
            }

            newValue = boolVal ? 1 : 0; 
        } else {
            const stringVal = interaction.options.getString("string-value");
            if (typeof stringVal !== "string") {
                return await interaction.reply({ content: `Please provide a \`string-value\` for \`${variableKey}\`.`, flags: MessageFlags.Ephemeral });
            }

            newValue = stringVal;
        }

        database.run(`UPDATE servers SET ${dbColumn} = ? WHERE guildId = ? AND channelId = ?`, [newValue, interaction.guildId, interaction.channelId], async function (err) {
            if (err) {
                console.error(err)
                await interaction.reply({ content: "Internal Server Error", flags: MessageFlags.Ephemeral });
                return;
            }

            await interaction.reply({ content: `Successfully updated \`${variableKey}\` to \`${newValue}\`.`, flags: MessageFlags.Ephemeral });
        });
    }
};
