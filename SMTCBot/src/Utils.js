module.exports = {
    convertTo2DArray(arr, itemsPerRow) {
        const result = [];
        for (let i = 0; i < arr.length; i += itemsPerRow) {
          result.push(arr.slice(i, i + itemsPerRow));
        }
        return result;
    }
    
}