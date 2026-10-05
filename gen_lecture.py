# -*- coding: utf-8 -*-
# 生成外部中断学习讲义HTML
html = r'''<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>单片机打卡天天练 · 外部中断驱动工作台控制 · Step-by-Step 讲义</title>
<style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  html, body {
    font-family: -apple-system, "PingFang SC", "Microsoft YaHei", "Helvetica Neue", sans-serif;
    line-height: 1.75; color: #2c3e50;
    background: linear-gradient(135deg, #f5f7fa 0%, #e8ecf3 100%);
    scroll-behavior: smooth;
  }
  .container { max-width: 1080px; margin: 0 auto; padding: 24px; }
  header.hero {
    background: linear-gradient(135deg, #1e3c72 0%, #2a5298 50%, #3a7bd5 100%);
    color: white; padding: 36px 40px; border-radius: 18px;
    box-shadow: 0 10px 30px rgba(30,60,114,0.25); margin-bottom: 28px;
  }
  header.hero h1 { font-size: 26px; margin-bottom: 8px; letter-spacing: 1px; }
  header.hero .subtitle { font-size: 15px; opacity: 0.85; margin-bottom: 16px; }
  header.hero .meta-row { display: flex; gap: 12px; flex-wrap: wrap; }
  header.hero .meta-tag {
    background: rgba(255,255,255,0.15); padding: 4px 14px; border-radius: 14px;
    font-size: 13px; letter-spacing: 1px;
  }
  .card {
    background: white; border-radius: 14px; padding: 28px 32px;
    margin-bottom: 24px; box-shadow: 0 4px 16px rgba(0,0,0,0.06);
  }
  .card h2 {
    font-size: 22px; color: #1e3c72; margin-bottom: 16px;
    padding-bottom: 10px; border-bottom: 3px solid #3a7bd5;
  }
  .card h3 { font-size: 17px; color: #2a5298; margin: 18px 0 10px; }
  .card p { margin-bottom: 10px; }
  .card ul, .card ol { margin: 8px 0 12px 24px; }
  .card li { margin-bottom: 6px; }
  .step-card {
    background: white; border-radius: 14px; margin-bottom: 24px;
    box-shadow: 0 4px 16px rgba(0,0,0,0.06); overflow: hidden;
  }
  .step-header {
    background: linear-gradient(135deg, #2a5298, #3a7bd5);
    color: white; padding: 16px 32px; display: flex; align-items: center; gap: 14px;
  }
  .step-badge {
    background: rgba(255,255,255,0.2); width: 42px; height: 42px; border-radius: 50%;
    display: flex; align-items: center; justify-content: center; font-size: 18px; font-weight: bold;
    flex-shrink: 0;
  }
  .step-header .step-title { font-size: 18px; font-weight: 600; }
  .step-header .step-goal { font-size: 13px; opacity: 0.8; margin-top: 2px; }
  .step-body { padding: 24px 32px; }
  .dialog-block { margin: 16px 0; }
  .dialog-user {
    background: #e8f4fd; border-left: 4px solid #3a7bd5;
    padding: 14px 18px; border-radius: 0 10px 10px 0; margin-bottom: 12px;
  }
  .dialog-ai {
    background: #f0f9f0; border-left: 4px solid #52c41a;
    padding: 14px 18px; border-radius: 0 10px 10px 0;
  }
  .dialog-label {
    font-size: 13px; font-weight: 600; margin-bottom: 6px; display: flex; align-items: center; gap: 6px;
  }
  .dialog-user .dialog-label { color: #2a5298; }
  .dialog-ai .dialog-label { color: #389e0d; }
  .dialog-content { font-size: 14px; color: #333; }
  .dialog-content .quote {
    background: rgba(0,0,0,0.04); padding: 10px 14px; border-radius: 8px;
    font-style: italic; margin-top: 6px; border: 1px dashed #ccc;
  }
  .tip-box {
    background: #fffbe6; border: 2px solid #ffe58f; border-radius: 10px;
    padding: 16px 20px; margin: 14px 0;
  }
  .tip-box .tip-title {
    color: #d48806; font-weight: 600; font-size: 15px; margin-bottom: 8px;
    display: flex; align-items: center; gap: 6px;
  }
  .tip-box ul { margin: 6px 0 0 20px; }
  .insight-box {
    background: #f6ffed; border: 2px solid #b7eb8f; border-radius: 10px;
    padding: 16px 20px; margin: 14px 0;
  }
  .insight-box .insight-title {
    color: #389e0d; font-weight: 600; font-size: 15px; margin-bottom: 8px;
    display: flex; align-items: center; gap: 6px;
  }
  pre {
    background: #282c34; color: #abb2bf; padding: 16px 20px; border-radius: 10px;
    font-family: "Consolas", "Courier New", monospace; font-size: 13px;
    overflow-x: auto; margin: 12px 0; line-height: 1.5;
  }
  pre .kw { color: #c678dd; }
  pre .str { color: #98c379; }
  pre .com { color: #5c6370; font-style: italic; }
  pre .fn { color: #61afef; }
  pre .num { color: #d19a66; }
  code {
    background: rgba(0,0,0,0.06); padding: 2px 6px; border-radius: 4px;
    font-family: "Consolas", monospace; font-size: 13px; color: #c7254e;
  }
  table {
    width: 100%; border-collapse: collapse; margin: 14px 0; font-size: 14px;
  }
  th, td { border: 1px solid #e0e0e0; padding: 8px 12px; text-align: left; }
  th { background: #f0f4f8; color: #1e3c72; font-weight: 600; }
  tr:nth-child(even) { background: #fafbfc; }
  .oled-screen {
    background: #1a1a2e; color: #00ff88; font-family: monospace; font-size: 15px;
    padding: 20px 28px; border-radius: 10px; margin: 14px 0;
    box-shadow: inset 0 0 20px rgba(0,255,136,0.1); line-height: 1.8;
    border: 2px solid #333; white-space: pre;
  }
  .oled-label { font-size: 13px; color: #888; margin-bottom: 4px; }
  .flow-row { display: flex; align-items: center; gap: 8px; flex-wrap: wrap; margin: 14px 0; }
  .flow-item {
    background: #e8f4fd; border: 2px solid #3a7bd5; border-radius: 8px;
    padding: 6px 16px; font-size: 14px; color: #2a5298; font-weight: 500;
  }
  .flow-arrow { color: #3a7bd5; font-size: 20px; font-weight: bold; }
  .summary-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 16px; margin: 16px 0; }
  .summary-item {
    background: #f8f9fb; border-radius: 10px; padding: 16px 20px; border: 1px solid #e0e6ed;
  }
  .summary-item h4 { color: #2a5298; font-size: 15px; margin-bottom: 8px; }
  .summary-item p { font-size: 13px; color: #555; }
  .toc {
    background: white; border-radius: 14px; padding: 24px 32px; margin-bottom: 24px;
    box-shadow: 0 4px 16px rgba(0,0,0,0.06);
  }
  .toc h2 { font-size: 20px; color: #1e3c72; margin-bottom: 14px; }
  .toc ol { margin-left: 24px; }
  .toc li { margin-bottom: 8px; }
  .toc a { color: #3a7bd5; text-decoration: none; }
  .toc a:hover { text-decoration: underline; }
  footer { text-align: center; padding: 20px; color: #999; font-size: 13px; }
</style>
</head>
<body>
<div class="container">

  <header class="hero">
    <h1>外部中断驱动工作台控制 · Step-by-Step 讲义</h1>
    <div class="subtitle">从急停电机到限位保护——STC15 外部中断的渐进式工程实践</div>
    <div class="meta-row">
      <span class="meta-tag">单片机打卡天天练</span>
      <span class="meta-tag">STC15F2K60S2</span>
      <span class="meta-tag">外部中断 INT0~INT4</span>
      <span class="meta-tag">工作台限位保护</span>
      <span class="meta-tag">面向初学者</span>
    </div>
  </header>

  <div class="toc">
    <h2>📋 讲义目录</h2>
    <ol>
      <li><a href="#overview">项目概述与最终效果</a></li>
      <li><a href="#knowledge">外部中断知识铺垫</a></li>
      <li><a href="#principles">与 AI 对话的核心原则</a></li>
      <li><a href="#step1">Step 1 — 初识外部中断：INT0 急停电机</a></li>
      <li><a href="#step2">Step 2 — 四中断综合：计数+清零+正转+刹车（含位号纠正）</a></li>
      <li><a href="#step3">Step 3 — 引脚释放艺术：DIR 改道与工作台限位保护</a></li>
      <li><a href="#step4">Step 4 — 限位刹车 + 字库扩展 + 中文显示</a></li>
      <li><a href="#step5">Step 5 — 工程完善：限位互锁与报警自动解除</a></li>
      <li><a href="#result">最终项目成果展示</a></li>
      <li><a href="#summary">AI 对话经验总结</a></li>
    </ol>
  </div>

  <div class="card" id="overview">
    <h2>一、项目概述与最终效果</h2>
    <h3>1.1 项目背景</h3>
    <p>前序项目已实现电机的自动/点动双模式控制。本次通过 <strong>5 轮渐进式对话</strong>，从外部中断的
       概念入门开始，逐步构建出一个模拟工业工作台的<strong>限位保护控制系统</strong>——电机驱动工作台
       往复运动，两端限位开关通过外部中断实时检测，碰到限位立即刹车并报警，只能反方向脱离。</p>
    <p>整个过程经历了：单中断急停 → 四中断综合 → 引脚冲突解决 → 限位保护 → 工程逻辑完善，
       每一步都引入了新的知识点，是外部中断从"会用"到"用好"的完整学习路径。</p>

    <h3>1.2 最终实现的功能</h3>
    <table>
      <tr><th>功能</th><th>实现方式</th><th>说明</th></tr>
      <tr><td>双模式控制</td><td>S7 切换自动/点动</td><td>继承前序项目，含方向互锁、模式切换保护</td></tr>
      <tr><td>左限位保护</td><td>INT2 (P3.6) 外部中断</td><td>碰到左限位 → 刹车停转 + 置报警，禁止左行</td></tr>
      <tr><td>右限位保护</td><td>INT3 (P3.7) 外部中断</td><td>碰到右限位 → 刹车停转 + 置报警，禁止右行</td></tr>
      <tr><td>报警自动解除</td><td>反方向启动时 alarm=0</td><td>左限位时按右行键脱离，报警自动清除</td></tr>
      <tr><td>运行状态显示</td><td>OLED 第二行</td><td>右行/左行/报警/停止 四态综合显示</td></tr>
      <tr><td>限位状态显示</td><td>OLED 第四行</td><td>正常/左侧/右侧 实时反映限位开关状态</td></tr>
    </table>

    <h3>1.3 最终 OLED 显示效果</h3>
    <div class="oled-label">正常运行（自动模式，右行中）：</div>
    <div class="oled-screen">    电机控制系统
运行状态：右行
当前模式：自动
限位状态：正常</div>
    <div class="oled-label">左限位触发（报警状态）：</div>
    <div class="oled-screen">    电机控制系统
运行状态：报警
当前模式：自动
限位状态：左侧</div>

    <h3>1.4 开发流程全景</h3>
    <div class="flow-row">
      <span class="flow-item">Step 1: INT0 急停</span>
      <span class="flow-arrow">→</span>
      <span class="flow-item">Step 2: 四中断综合</span>
      <span class="flow-arrow">→</span>
      <span class="flow-item">Step 3: 引脚释放+限位</span>
      <span class="flow-arrow">→</span>
      <span class="flow-item">Step 4: 刹车+中文化</span>
      <span class="flow-arrow">→</span>
      <span class="flow-item">Step 5: 工程完善</span>
    </div>
  </div>

  <div class="card" id="knowledge">
    <h2>二、外部中断知识铺垫</h2>
    <p>STC15F2K60S2 提供 5 个外部中断 INT0~INT4，它们分为两类，配置方式不同，这是本项目的核心知识点。</p>

    <h3>2.1 两类外部中断对比</h3>
    <table>
      <tr><th>特性</th><th>INT0 / INT1（标准中断）</th><th>INT2 / INT3 / INT4（扩展中断）</th></tr>
      <tr><td>引脚</td><td>P3.2 / P3.3</td><td>P3.6 / P3.7 / P3.0</td></tr>
      <tr><td>向量号</td><td>0 / 2</td><td>10 / 11 / 16</td></tr>
      <tr><td>触发方式</td><td>可选：ITx=0 低电平，ITx=1 下降沿</td><td>仅下降沿触发</td></tr>
      <tr><td>允许位寄存器</td><td>IE（可位寻址）</td><td>INT_CLKO（0x8F，不可位寻址）</td></tr>
      <tr><td>允许位操作</td><td><code>EX0=1;</code> 直接置位</td><td><code>INT_CLKO |= 0x10;</code> 整体或运算</td></tr>
    </table>

    <h3>2.2 INT_CLKO 寄存器位号（关键易错点）</h3>
    <div class="tip-box">
      <div class="tip-title">⚠️ 位号必须记准</div>
      <ul>
        <li><strong>EX2 = bit4</strong> (0x10) — INT2 中断使能</li>
        <li><strong>EX3 = bit5</strong> (0x20) — INT3 中断使能</li>
        <li><strong>EX4 = bit6</strong> (0x40) — INT4 中断使能</li>
      </ul>
      <p style="margin-top:8px;">本项目中 AI 曾把 EX2/EX4 位号写反，被用户纠正。这个错误很隐蔽——
         代码能编译通过，但中断永远不触发。教训：寄存器位号必须查权威数据手册确认。</p>
    </div>

    <h3>2.3 volatile 关键字</h3>
    <p>跨中断共享的变量（如 <code>alarm</code>、<code>state</code>）必须用 <code>volatile</code> 声明。
       否则编译器可能将其优化到寄存器，导致中断里修改后主程序读不到新值。这是嵌入式编程的必备知识。</p>
  </div>

  <div class="card" id="principles">
    <h2>三、与 AI 对话的核心原则</h2>
    <p>本次外部中断学习过程中，以下 5 条原则贯穿始终：</p>
    <div class="tip-box">
      <div class="tip-title">💡 五条核心原则</div>
      <ul>
        <li><strong>原则 1：场景驱动</strong> — 用实际工程场景（工作台限位保护）引出技术需求，AI 会自动设计合理的硬件接线和控制逻辑</li>
        <li><strong>原则 2：及时纠错</strong> — AI 写错寄存器位号时，直接给出正确值"EX2=bit4, EX4=bit6"，AI 立即修正并更新注释</li>
        <li><strong>原则 3：增量迭代</strong> — 每轮只加一个功能点（急停→四中断→限位→刹车→互锁），逐步完善</li>
        <li><strong>原则 4：明确约束</strong> — "限位时只能反方向走"这类工程约束直接说清楚，AI 会翻译成 if 条件</li>
        <li><strong>原则 5：要求解释</strong> — 每次修改后 AI 都输出修改说明和教学注释，可直接用于课堂</li>
      </ul>
    </div>
  </div>

  <!-- Step 1 -->
  <div class="step-card" id="step1">
    <div class="step-header">
      <div class="step-badge">1</div>
      <div>
        <div class="step-title">初识外部中断：INT0 急停电机</div>
        <div class="step-goal">目标：学习外部中断概念，用 INT0 实现按键急停电机</div>
      </div>
    </div>
    <div class="step-body">
      <div class="dialog-block">
        <div class="dialog-user">
          <div class="dialog-label">🙋 用户提问</div>
          <div class="dialog-content">
            <div class="quote">"学习外部中断，设计 INT0 急停电机的案例。主程序做 2 秒延时演示，INT0(P3.2=S5) 中断停止电机。"</div>
          </div>
        </div>
        <div class="dialog-ai">
          <div class="dialog-label">🤖 AI 响应要点</div>
          <div class="dialog-content">
            <ul>
              <li>将原双模式控制 main.c <strong>备份</strong>到根目录 <code>main_双模式控制.c</code>，保留学习成果</li>
              <li>设计新 main.c：主循环每 2 秒翻转一次电机（演示），<code>DelayMs</code> 参数是 unsigned char（最大255），2秒需循环 20 次 <code>DelayMs(100)</code></li>
              <li>配置 INT0：<code>IT0=1;</code>（下降沿）<code>EX0=1;</code>（允许）<code>EA=1;</code>（总中断）</li>
              <li>中断函数：<code>void Int0_Routine(void) interrupt 0</code>，调用 <code>Motor_Coast()</code> 急停</li>
            </ul>
            <pre><span class="com">/* INT0 中断：S5 按下立即急停 */</span>
<span class="kw">void</span> <span class="fn">Int0_Routine</span>(<span class="kw">void</span>) interrupt <span class="num">0</span>
{
    <span class="fn">Motor_Coast</span>();   <span class="com">/* 立即惯性停转 */</span>
}

<span class="kw">void</span> <span class="fn">main</span>(<span class="kw">void</span>)
{
    <span class="fn">Motor_Init</span>();
    IT0 = <span class="num">1</span>;  EX0 = <span class="num">1</span>;  EA = <span class="num">1</span>;  <span class="com">/* INT0 下降沿触发 */</span>
    <span class="kw">while</span> (<span class="num">1</span>) {
        <span class="fn">Motor_Forward</span>();
        <span class="kw">for</span> (i=<span class="num">0</span>; i&lt;<span class="num">20</span>; i++) <span class="fn">DelayMs</span>(<span class="num">100</span>);  <span class="com">/* 2秒 */</span>
        <span class="fn">Motor_Reverse</span>();
        <span class="kw">for</span> (i=<span class="num">0</span>; i&lt;<span class="num">20</span>; i++) <span class="fn">DelayMs</span>(<span class="num">100</span>);
    }
}</pre>
          </div>
        </div>
      </div>
      <div class="tip-box">
        <div class="tip-title">💡 对话技巧分析</div>
        <ul>
          <li><strong>"学习外部中断"</strong> — 明确这是学习目的，AI 会从概念讲起，配置代码加详细注释，而非只给裸代码</li>
          <li><strong>"主程序做2秒延时演示"</strong> — 指定主循环行为，让中断的效果可见：主程序忙着延时，中断随时打断。这正好体现中断的<strong>实时性</strong></li>
          <li>AI 主动<strong>备份原文件</strong>而非覆盖，体现了对学习成果的保护——每个阶段的代码都留底，方便回溯对比</li>
        </ul>
      </div>
      <div class="insight-box">
        <div class="insight-title">📝 教学启示</div>
        <p>这一步的核心概念是<strong>"中断打断主程序"</strong>。主程序在 2 秒延时里"什么都做不了"（阻塞），
           但按下 S5 后中断<strong>立即</strong>响应——这就是中断相对于轮询的根本优势：<strong>响应不依赖主程序状态</strong>。
           学生应对比理解：如果用轮询检测 S5，在 DelayMs 期间是检测不到的。</p>
      </div>
    </div>
  </div>

  <!-- Step 2 -->
  <div class="step-card" id="step2">
    <div class="step-header">
      <div class="step-badge">2</div>
      <div>
        <div class="step-title">四中断综合：计数+清零+正转+刹车（含位号纠正）</div>
        <div class="step-goal">目标：同时使用 4 个外部中断实现不同功能，掌握扩展中断配置</div>
      </div>
    </div>
    <div class="step-body">
      <div class="dialog-block">
        <div class="dialog-user">
          <div class="dialog-label">🙋 用户提问</div>
          <div class="dialog-content">
            <div class="quote">"用 5 个外部中断设计：INT4 计数、INT2 清零、INT0 正转、INT1 刹车。"</div>
          </div>
        </div>
        <div class="dialog-ai">
          <div class="dialog-label">🤖 AI 响应要点</div>
          <div class="dialog-content">
            <ul>
              <li>分析：INT3 (P3.7) 被电机 DIR 占用，实际只能用 4 个中断（INT0/1/2/4）</li>
              <li>INT0/INT1 用 IE 寄存器：<code>IT0=1; EX0=1;</code> / <code>IT1=1; EX1=1;</code></li>
              <li>INT2/INT4 用 INT_CLKO 寄存器：<code>INT_CLKO |= 0x10;</code> / <code>INT_CLKO |= 0x40;</code></li>
              <li>主循环只刷显示，所有控制逻辑在中断里完成；<code>count</code>、<code>motorState</code> 用 <code>volatile</code></li>
            </ul>
          </div>
        </div>
      </div>
      <div class="dialog-block">
        <div class="dialog-user">
          <div class="dialog-label">🙋 用户纠正（关键！）</div>
          <div class="dialog-content">
            <div class="quote">"INT_CLKO 位号写反了：EX2=bit4，EX4=bit6，不是你写的 EX2=bit6、EX4=bit4。"</div>
          </div>
        </div>
        <div class="dialog-ai">
          <div class="dialog-label">🤖 AI 响应要点</div>
          <div class="dialog-content">
            <ul>
              <li>立即修正：<code>INT_CLKO |= 0x10;</code>（EX2=bit4）和 <code>INT_CLKO |= 0x40;</code>（EX4=bit6）</li>
              <li>更新所有注释中的位号说明，确保代码与注释一致</li>
              <li>将正确位号记录到记忆文件，避免后续再犯</li>
            </ul>
          </div>
        </div>
      </div>
      <div class="tip-box">
        <div class="tip-title">💡 对话技巧分析</div>
        <ul>
          <li><strong>AI 主动分析引脚冲突</strong> — INT3 在 P3.7，而 DIR 也在 P3.7，AI 没有强行使用而是说明冲突，只用了 4 个中断。这种<strong>约束意识</strong>是工程思维</li>
          <li><strong>用户纠正位号</strong> — 这是本步最关键的一刻。AI 写反了 EX2/EX4 的位号，代码能编译但中断不触发。用户凭借数据手册知识纠正，AI 立即修正。<strong>这说明 AI 的硬件知识需要人工校验</strong></li>
          <li><strong>两类中断配置差异</strong> — INT0/1 用 IE 寄存器（可位寻址，<code>EX0=1</code>），INT2/3/4 用 INT_CLKO（不可位寻址，<code>|=</code>）。AI 在注释中详细对比，适合教学</li>
        </ul>
      </div>
      <div class="insight-box">
        <div class="insight-title">📝 教学启示</div>
        <p>这一步有两个重点：① <strong>寄存器位号必须查手册</strong>，AI 可能记错，代码能编译但行为错误，这种 bug 极难发现；
           ② <strong>不可位寻址寄存器</strong>用 <code>|=</code> 而非 <code>=</code>，避免覆盖其他位。学生应养成习惯：
           涉及寄存器位操作时，先查手册确认位号，再写代码。</p>
      </div>
    </div>
  </div>

  <!-- Step 3 -->
  <div class="step-card" id="step3">
    <div class="step-header">
      <div class="step-badge">3</div>
      <div>
        <div class="step-title">引脚释放艺术：DIR 改道与工作台限位保护</div>
        <div class="step-goal">目标：释放 P3.7 给 INT3，实现左右限位中断保护</div>
      </div>
    </div>
    <div class="step-body">
      <div class="dialog-block">
        <div class="dialog-user">
          <div class="dialog-label">🙋 用户提问</div>
          <div class="dialog-content">
            <div class="quote">"做工作台控制：DIR 改到 P0.4 释放 P3.7 给 INT3。INT2 左限位、INT3 右限位，限位触发惯性停止+报警显示。"</div>
          </div>
        </div>
        <div class="dialog-ai">
          <div class="dialog-label">🤖 AI 响应要点</div>
          <div class="dialog-content">
            <ul>
              <li>修改 motor.c：DIR 从 P3.7 改到 P0.4，更新端口配置和注释</li>
              <li>新增 <code>volatile unsigned char alarm</code>：0=正常，1=左限位，2=右限位</li>
              <li>INT2 中断：<code>Motor_Coast(); state=0; alarm=1;</code></li>
              <li>INT3 中断：<code>Motor_Coast(); state=0; alarm=2;</code></li>
              <li>第四行用英文 <code>"Limit:OK/Left/Right"</code> 显示（因字库缺中文）</li>
            </ul>
            <pre><span class="com">/* INT2：左限位 → 惯性停转 + 报警 */</span>
<span class="kw">void</span> <span class="fn">Int2_Routine</span>(<span class="kw">void</span>) interrupt <span class="num">10</span>
{
    <span class="fn">Motor_Coast</span>();
    state = <span class="num">0</span>;
    alarm = <span class="num">1</span>;  <span class="com">/* 置左限位报警 */</span>
}</pre>
          </div>
        </div>
      </div>
      <div class="tip-box">
        <div class="tip-title">💡 对话技巧分析</div>
        <ul>
          <li><strong>"DIR 改到 P0.4 释放 P3.7"</strong> — 用户主动提出引脚重新分配方案。这体现了单片机开发的核心技能：<strong>引脚统筹</strong>。同一引脚不能复用，遇到冲突必须重新规划</li>
          <li><strong>AI 用英文显示报警</strong> — 字库缺"限、位、左、右"等字，AI 没有强行显示中文（会乱码），而是用英文替代并说明原因。这种<strong>退而求其次</strong>的务实做法值得学习</li>
          <li><strong>volatile 修饰 alarm</strong> — alarm 在中断里写入、在主循环里读取，跨上下文共享必须 volatile。AI 主动添加并注释"必须 volatile"</li>
        </ul>
      </div>
      <div class="insight-box">
        <div class="insight-title">📝 教学启示</div>
        <p>这一步引入了<strong>引脚复用冲突</strong>的概念。单片机引脚有限，功能多了就会打架。解决方案是
           <strong>重新分配引脚</strong>——把 DIR 从 P3.7 挪到 P0.4，腾出 P3.7 给 INT3。学生应理解：
           选引脚时不能只看单个功能，要全局统筹，避免两个功能抢同一个引脚。</p>
      </div>
    </div>
  </div>

  <!-- Step 4 -->
  <div class="step-card" id="step4">
    <div class="step-header">
      <div class="step-badge">4</div>
      <div>
        <div class="step-title">限位刹车 + 字库扩展 + 中文显示</div>
        <div class="step-goal">目标：限位改紧急刹车，扩展字库，第四行改中文显示</div>
      </div>
    </div>
    <div class="step-body">
      <div class="dialog-block">
        <div class="dialog-user">
          <div class="dialog-label">🙋 用户提问</div>
          <div class="dialog-content">
            <div class="quote">"限位应该紧急刹车停止（不是惯性停止）。字库不够就做字库。第四行显示：限位状态：正常/左侧/右侧。"</div>
          </div>
        </div>
        <div class="dialog-ai">
          <div class="dialog-label">🤖 AI 响应要点</div>
          <div class="dialog-content">
            <ul>
              <li>将 Int2/Int3 中的 <code>Motor_Coast()</code> 改为 <code>Motor_Brake()</code>（刹车制动，工作台限位需快速停下）</li>
              <li>用 <strong>Python + PIL</strong> 从系统宋体生成 16×16 字模，先用"正"字与字库对比<strong>验证取模方式</strong>（列行式、低位在上），完全匹配</li>
              <li>生成"限、常、左、侧、右"5 个字的字模，插入 OLED_Data.c</li>
              <li>第四行改为 <code>OLED_ShowMixedText(0,48,"限位状态:")</code> + <code>OLED_ShowChinese(72,48,"正常")</code></li>
            </ul>
            <pre><span class="com">/* Python 生成字模并验证取模方式 */</span>
<span class="com"># 渲染"正"字到16x16，按列行式低位在上提取</span>
<span class="com"># 与字库对比 → 完全匹配 → 取模方式正确</span>
<span class="str">"正"</span> → 0x00,0x02,0x02,0xC2,...  <span class="com"># 与字库一致 ✓</span>
<span class="str">"限"</span> → 0x00,0xFF,0x11,0x2D,...  <span class="com"># 新生成</span></pre>
          </div>
        </div>
      </div>
      <div class="tip-box">
        <div class="tip-title">💡 对话技巧分析</div>
        <ul>
          <li><strong>"字库不够就做字库"</strong> — 这句话让 AI 从"将就英文"切换到"主动扩展字库"。AI 用 Python+PIL 自动生成字模，这是 AI 的突出能力——<strong>调用工具解决依赖问题</strong></li>
          <li><strong>先验证再生成</strong> — AI 没有直接生成5个字模，而是先用"正"字（字库已有）验证取模方式是否正确，确认后再批量生成。这种<strong>先校验后生产</strong>的严谨做法避免了批量错误</li>
          <li><strong>惯性 vs 刹车</strong> — 用户指出限位应"紧急刹车"而非"惯性停止"。Motor_Coast() 是断电滑行，Motor_Brake() 是制动快速停。工程上限位保护必须快速停，这个区分体现了<strong>物理动作的工程含义</strong></li>
        </ul>
      </div>
      <div class="insight-box">
        <div class="insight-title">📝 教学启示</div>
        <p>这一步展示了<strong>字模生成</strong>的完整流程：① 确定取模方式（列行式/行列式、位序）② 用已知字模验证
           ③ 批量生成新字模 ④ 插入字库数组。学生应理解：OLED 显示中文的本质是查字模表，缺字就补字模，
           而非"显示不了就算了"。同时要区分 Motor_Coast（惯性）和 Motor_Brake（刹车）的工程差异。</p>
      </div>
    </div>
  </div>

  <!-- Step 5 -->
  <div class="step-card" id="step5">
    <div class="step-header">
      <div class="step-badge">5</div>
      <div>
        <div class="step-title">工程完善：限位互锁与报警自动解除</div>
        <div class="step-goal">目标：限位互锁、反方向解除报警、运行状态综合显示</div>
      </div>
    </div>
    <div class="step-body">
      <div class="dialog-block">
        <div class="dialog-user">
          <div class="dialog-label">🙋 用户提问</div>
          <div class="dialog-content">
            <div class="quote">"结合实际工程完善：①左限位时不能左行只能右行，反方向移动后报警自动解除；②解除报警后显示恢复正常；③运行状态显示右行/左行/报警/停止。"</div>
          </div>
        </div>
        <div class="dialog-ai">
          <div class="dialog-label">🤖 AI 响应要点</div>
          <div class="dialog-content">
            <ul>
              <li><strong>限位互锁</strong>：S4 右行加 <code>alarm != 2</code>（右限位禁右行），S6 左行加 <code>alarm != 1</code>（左限位禁左行）</li>
              <li><strong>报警自动解除</strong>：启动成功后 <code>alarm = 0;</code>，脱离限位即清除</li>
              <li><strong>运行状态四态显示</strong>：state=1→"右行"，state=2→"左行"，state=0且alarm!=0→"报警"，否则→"停止"</li>
              <li>补充"报、警"字模；方向约定：正转=右行，反转=左行</li>
            </ul>
            <pre><span class="com">/* S4 右行：非左行中 且 非右限位才允许 */</span>
<span class="kw">if</span> (state != <span class="num">2</span> && alarm != <span class="num">2</span>) {
    <span class="fn">Motor_Forward</span>();
    state = <span class="num">1</span>;
    alarm = <span class="num">0</span>;  <span class="com">/* 脱离限位，自动解除报警 */</span>
}

<span class="com">/* 运行状态四态综合显示 */</span>
<span class="kw">if</span> (state == <span class="num">1</span>)      <span class="fn">OLED_ShowChinese</span>(<span class="num">72</span>,<span class="num">16</span>,<span class="str">"右行"</span>);
<span class="kw">else if</span> (state == <span class="num">2</span>) <span class="fn">OLED_ShowChinese</span>(<span class="num">72</span>,<span class="num">16</span>,<span class="str">"左行"</span>);
<span class="kw">else if</span> (alarm != <span class="num">0</span>) <span class="fn">OLED_ShowChinese</span>(<span class="num">72</span>,<span class="num">16</span>,<span class="str">"报警"</span>);
<span class="kw">else</span>                <span class="fn">OLED_ShowChinese</span>(<span class="num">72</span>,<span class="num">16</span>,<span class="str">"停止"</span>);</pre>
          </div>
        </div>
      </div>
      <div class="tip-box">
        <div class="tip-title">💡 对话技巧分析</div>
        <ul>
          <li><strong>"结合实际工程完善"</strong> — 这个开头把需求从"能跑"提升到"工程可用"。AI 据此设计了互锁、自动解除、综合显示三个机制，而非只加一个 if</li>
          <li><strong>三条需求编号列出</strong> — ①②③分条描述，AI 逐条实现并对应验证。分条让需求不遗漏，也让 AI 的响应可逐条检查</li>
          <li><strong>"反方向移动后报警自动解除"</strong> — 这是工程逻辑的精髓。AI 把它翻译成：启动成功时 <code>alarm=0</code>。即只要成功向反方向走了，就认为已脱离限位。简洁有效</li>
          <li><strong>运行状态四态</strong> — 不是简单的正转/反转/停止，而是加入了"报警"态。state=0 且 alarm!=0 显示"报警"，这需要<strong>综合两个变量判断</strong>，是状态机思维的进阶</li>
        </ul>
      </div>
      <div class="insight-box">
        <div class="insight-title">📝 教学启示</div>
        <p>这一步是整个项目的<strong>工程化升华</strong>。学生应重点理解三个概念：① <strong>互锁</strong>——限位方向
           禁止继续走，这是安全保护的核心；② <strong>自动解除</strong>——报警不需要手动清除，反方向脱离即恢复，
           这符合操作直觉；③ <strong>多变量综合状态</strong>——显示"报警"需要同时看 state 和 alarm 两个变量，
           这是状态机从单变量到多变量的进阶。至此，程序从"能演示"变成了"能上机"。</p>
      </div>
    </div>
  </div>

  <!-- 最终成果 -->
  <div class="card" id="result">
    <h2>九、最终项目成果展示</h2>
    <h3>9.1 最终中断配置表</h3>
    <table>
      <tr><th>中断</th><th>引脚</th><th>向量号</th><th>功能</th><th>配置</th></tr>
      <tr><td>INT2</td><td>P3.6</td><td>10</td><td>左限位 → 刹车+报警</td><td><code>INT_CLKO |= 0x10</code></td></tr>
      <tr><td>INT3</td><td>P3.7</td><td>11</td><td>右限位 → 刹车+报警</td><td><code>INT_CLKO |= 0x20</code></td></tr>
    </table>

    <h3>9.2 状态变量</h3>
    <table>
      <tr><th>变量</th><th>含义</th><th>取值</th><th>volatile</th></tr>
      <tr><td><code>mode</code></td><td>控制模式</td><td>1=自动，0=点动</td><td>否（主程序独占）</td></tr>
      <tr><td><code>state</code></td><td>电机状态</td><td>0=停止，1=右行，2=左行</td><td>是（中断写0）</td></tr>
      <tr><td><code>alarm</code></td><td>限位报警</td><td>0=正常，1=左限位，2=右限位</td><td>是（中断写，主循环清）</td></tr>
    </table>

    <h3>9.3 操作流程示例</h3>
    <table>
      <tr><th>步骤</th><th>操作</th><th>屏幕变化</th></tr>
      <tr><td>开机</td><td>—</td><td>运行状态：停止 / 当前模式：自动 / 限位状态：正常</td></tr>
      <tr><td>按 S4</td><td>右行</td><td>运行状态：右行</td></tr>
      <tr><td>碰到右限位</td><td>INT3 中断</td><td>运行状态：报警 / 限位状态：右侧（电机已刹车）</td></tr>
      <tr><td>按 S4</td><td>尝试右行</td><td>无效（右限位禁止右行）</td></tr>
      <tr><td>按 S6</td><td>左行脱离</td><td>运行状态：左行 / 限位状态：正常（报警自动解除）</td></tr>
      <tr><td>按 S5</td><td>停止</td><td>运行状态：停止</td></tr>
    </table>

    <h3>9.4 涉及的文件改动汇总</h3>
    <table>
      <tr><th>文件</th><th>改动内容</th></tr>
      <tr><td>hardware/motor.c / motor.h</td><td>DIR 从 P3.7 改到 P0.4，释放引脚给 INT3</td></tr>
      <tr><td>hardware/OLED_Data.c</td><td>补充 7 个汉字字模：限、常、左、侧、右、报、警</td></tr>
      <tr><td>user/main.c</td><td>工作台限位保护控制（INT2/INT3 中断 + 互锁 + 报警解除 + 四态显示）</td></tr>
      <tr><td>main_双模式控制.c</td><td>前序双模式控制代码备份（根目录）</td></tr>
    </table>
  </div>

  <!-- 经验总结 -->
  <div class="card" id="summary">
    <h2>十、AI 对话经验总结</h2>
    <h3>10.1 有效的提问模式</h3>
    <div class="summary-grid">
      <div class="summary-item">
        <h4>📌 场景驱动</h4>
        <p>"工作台限位保护" — 用工程场景引出需求，AI 自动设计合理架构</p>
      </div>
      <div class="summary-item">
        <h4>📌 主动纠错</h4>
        <p>"EX2=bit4, EX4=bit6" — AI 写错时直接给正确值，不解释为什么错</p>
      </div>
      <div class="summary-item">
        <h4>📌 分条列需求</h4>
        <p>"①②③" 编号列出 — AI 逐条实现，不遗漏</p>
      </div>
      <div class="summary-item">
        <h4>📌 明确工程约束</h4>
        <p>"限位时只能反方向走" — 约束直接说，AI 翻译成 if 条件</p>
      </div>
      <div class="summary-item">
        <h4>📌 指定物理动作</h4>
        <p>"紧急刹车不是惯性停止" — 区分 Motor_Brake 和 Motor_Coast</p>
      </div>
      <div class="summary-item">
        <h4>📌 允许扩展资源</h4>
        <p>"字库不够就做字库" — 授权 AI 主动补资源，而非将就</p>
      </div>
    </div>

    <h3>10.2 AI 协作的优势与局限</h3>
    <table>
      <tr><th>方面</th><th>AI 的优势</th><th>需要注意的局限</th></tr>
      <tr>
        <td>硬件配置</td>
        <td>能根据芯片特性自动选择配置方式（IE vs INT_CLKO）</td>
        <td>寄存器位号可能记错（EX2/EX4 写反），需人工校验</td>
      </tr>
      <tr>
        <td>引脚规划</td>
        <td>主动分析引脚冲突，提出重新分配方案</td>
        <td>需用户确认替代引脚可用（P0.4 是否空闲）</td>
      </tr>
      <tr>
        <td>字模生成</td>
        <td>Python+PIL 自动生成16×16字模，先验证取模方式再批量生成</td>
        <td>依赖系统字体（宋体），不同机器字形可能微差</td>
      </tr>
      <tr>
        <td>工程逻辑</td>
        <td>互锁、自动解除、四态显示等工程机制一次设计到位</td>
        <td>需用户用工程语言描述需求（"限位互锁"而非"加个if"）</td>
      </tr>
    </table>

    <h3>10.3 给学生的建议</h3>
    <div class="insight-box">
      <div class="insight-title">📝 四条核心建议</div>
      <ul>
        <li><strong>建议一：寄存器位号查手册</strong> — AI 可能记错位号，代码能编译但中断不触发。涉及寄存器位操作时，务必对照数据手册确认</li>
        <li><strong>建议二：理解中断的实时性</strong> — 中断的价值在于"打断主程序立即响应"，不依赖主循环状态。限位保护用中断而非轮询，就是为了保证响应速度</li>
        <li><strong>建议三：区分物理动作</strong> — Motor_Coast（惯性滑行）和 Motor_Brake（制动刹车）是不同的物理过程。限位保护要快速停，必须用刹车</li>
        <li><strong>建议四：工程思维=约束+自动恢复</strong> — 好的工程程序不是"能跑"，而是"有限制（互锁）+ 能恢复（自动解除）+ 可观测（状态显示）"。这三点齐了才算工程化</li>
      </ul>
    </div>
  </div>

  <footer>
    单片机打卡天天练 · 外部中断驱动工作台控制 · Step-by-Step 讲义<br>
    STC15F2K60S2 + Keil C51 + OLED 128×64 · 限位保护控制系统
  </footer>

</div>
</body>
</html>'''

with open(r"G:\25dpj\单片机打卡天天练\按键次数检测+oled显示\外部中断学习讲义.html", "w", encoding="utf-8") as f:
    f.write(html)
print("OK, len =", len(html))