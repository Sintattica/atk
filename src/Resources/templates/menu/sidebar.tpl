{atkconfig var="brand_logo" smartyvar="brand_logo"}
{atkconfig var="dispatcher" smartyvar="dispatcher"}

<aside class="main-sidebar {$sidebar_classes}">
    <!-- Brand Logo -->
    <a href="{$dispatcher}" class="brand-link {$brand_text_style}">
        <img src="{$brand_logo}" alt="ATK Logo" class="brand-image img-circle elevation-3" style="opacity: .8; max-width: 45px;">
        <span class="brand-text font-weight-light">{$app_title}</span>
    </a>

    <!-- Sidebar -->
    <div id="menu-sidebar" class="sidebar">
        <!-- Sidebar Menu -->
        <nav class="mt-2">
            <ul class="nav nav-pills nav-sidebar flex-column {$nav_sidebar_classes}" data-widget="treeview" role="menu" data-accordion="false">
                <!-- Add icons to the links using the .nav-icon class
                     with font-awesome or any other icon font library -->
                {$menu['sidebar']}
            </ul>
        </nav>
        <!-- /.sidebar-menu -->
    </div>
    <!-- /.sidebar -->
</aside>
