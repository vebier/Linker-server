function logWithSection(section, level, message, context = {}) {
    const timestamp = new Date().toISOString();
    
    // 构建要输出的字段（保持顺序）
    const fields = {
        timestamp,
        level: level.toUpperCase(),
        message,
        ...context
    };

    // 打印分隔线
    console.log(`${'='.repeat(8)}${section}${'='.repeat(8)}`);

    // 手动格式化每个字段，输出为带缩进的 JSON 属性（不带 {}）
    const indent = '  '; // 两个空格缩进
    const lines = Object.entries(fields).map(([key, value]) => {
        // JSON.stringify 用于保证字符串、null 等格式正确
        const jsonValue = JSON.stringify(value);
        return `${indent}"${key}": ${jsonValue},`;
    });

    // 去掉最后一行的逗号（可选），但保留也无妨（某些 JSON 允许尾逗号）
    // 如果你想去掉最后一行逗号：
    const lastLineIndex = lines.length - 1;
    if (lastLineIndex >= 0) {
        lines[lastLineIndex] = lines[lastLineIndex].slice(0, -1); // 去掉最后一个逗号
    }

    // 打印每一行
    lines.forEach(line => console.log(line));

    // 最后换行
    console.log();
}

module.exports = {logWithSection}