<div class="main-header navbar navbar-expand {$main_header_classes}">

    <!-- Left navbar links -->
    <ul class="navbar-nav navbar-left">

        {if !$hide_sidebar}
            <!-- Minimize Sidebar Button -->
            <li class="nav-item nav-minimize-sidebar">
                <a class="nav-link" data-widget="pushmenu" href="#" role="button"><i class="fas fa-bars"></i></a>
            </li>
        {else}

            <!-- Brand Logo -->
            <li class="nav-item nav-brand-logo">
                <a href="{$dispatcher}" class="mr-1">
                    <img src="{$brand_logo}" alt="ATK Logo" class="brand-image img-circle"
                         style="opacity: .8; max-width: 35px;">
                </a>
            </li>
        {/if}

        {$menu['left']}

    </ul>

    <!-- Right navbar links -->
    <ul class="navbar-nav navbar-right ml-auto">
        {$menu['right']}

        <li class="nav-item d-none">
            <a class="nav-link" data-widget="fullscreen" href="#" role="button">
                <i class="fas fa-expand-arrows-alt"></i>
            </a>
        </li>

    </ul>
</div>
